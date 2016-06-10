
#include <assert.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>

#include "linenoise.h"
#include "greet.h"
#include "sjs/sjs.h"

#define SJS_CLI_STDIN_BUF_SIZE    65536


typedef enum {
    SJS_CLI_REPL = 0,
    SJS_CLI_FILE,
    SJS_CLI_EVAL,
    SJS_CLI_STDIN
} cli_mode_t;

typedef struct {
    sjs_vm_t* vm;
    struct {
        int use_strict;
        char* data;
        int interactive;
        cli_mode_t mode;
    } options;
    int got_sigint;
} cli_t;

static cli_t cli;


static int completion_idpart(unsigned char c) {
    /* Very simplified "is identifier part" check. */
    if ((c >= (unsigned char) 'a' && c <= (unsigned char) 'z') ||
        (c >= (unsigned char) 'A' && c <= (unsigned char) 'Z') ||
        (c >= (unsigned char) '0' && c <= (unsigned char) '9') ||
        c == (unsigned char) '$' || c == (unsigned char) '_') {
        return 1;
    }
    return 0;
}


static int completion_digit(unsigned char c) {
    return (c >= (unsigned char) '0' && c <= (unsigned char) '9');
}


static duk_ret_t linenoise_completion_lookup(duk_context *ctx) {
    duk_size_t len;
    const char *orig;
    const unsigned char *p;
    const unsigned char *p_curr;
    const unsigned char *p_end;
    const char *key;
    const char *prefix;
    linenoiseCompletions *lc;
    duk_idx_t idx_obj;

    orig = duk_require_string(ctx, -3);
    p_curr = (const unsigned char *) duk_require_lstring(ctx, -2, &len);
    p_end = p_curr + len;
    lc = duk_require_pointer(ctx, -1);

    duk_push_global_object(ctx);
    idx_obj = duk_require_top_index(ctx);

    while (p_curr <= p_end) {
        /* p_curr == p_end allowed on purpose, to handle 'Math.' for example. */
        p = p_curr;
        while (p < p_end && p[0] != (unsigned char) '.') {
            p++;
        }
        /* 'p' points to a NUL (p == p_end) or a period. */
        prefix = duk_push_lstring(ctx, (const char *) p_curr, (duk_size_t) (p - p_curr));

#ifdef SJS_CLI_DEBUG
        fprintf(stderr, "Completion check: '%s'\n", prefix);
        fflush(stderr);
#endif

        if (p == p_end) {
            /* 'idx_obj' points to the object matching the last
             * full component, use [p_curr,p[ as a filter for
             * that object.
             */

            duk_enum(ctx, idx_obj, DUK_ENUM_INCLUDE_NONENUMERABLE);
            while (duk_next(ctx, -1, 0 /*get_value*/)) {
                key = duk_get_string(ctx, -1);
#ifdef SJS_CLI_DEBUG
                fprintf(stderr, "Key: %s\n", key ? key : "");
                fflush(stderr);
#endif
                if (!key) {
                    /* Should never happen, just in case. */
                    goto next;
                }

                /* Ignore array index keys: usually not desirable, and would
                 * also require ['0'] quoting.
                 */
                if (completion_digit(key[0])) {
                    goto next;
                }

                /* XXX: There's no key quoting now, it would require replacing the
                 * last component with a ['foo\nbar'] style lookup when appropriate.
                 */

                if (strlen(prefix) == 0) {
                    /* Partial ends in a period, e.g. 'Math.' -> complete all Math properties. */
                    duk_push_string(ctx, orig);  /* original, e.g. 'Math.' */
                    duk_push_string(ctx, key);
                    duk_concat(ctx, 2);
                    linenoiseAddCompletion(lc, duk_require_string(ctx, -1));
                    duk_pop(ctx);
                } else if (prefix && strcmp(key, prefix) == 0) {
                    /* Full completion, add a period, e.g. input 'Math' -> 'Math.'. */
                    duk_push_string(ctx, orig);  /* original, including partial last component */
                    duk_push_string(ctx, ".");
                    duk_concat(ctx, 2);
                    linenoiseAddCompletion(lc, duk_require_string(ctx, -1));
                    duk_pop(ctx);
                } else if (prefix && strncmp(key, prefix, strlen(prefix)) == 0) {
                    /* Last component is partial, complete. */
                    duk_push_string(ctx, orig);  /* original, including partial last component */
                    duk_push_string(ctx, key + strlen(prefix));  /* completion to last component */
                    duk_concat(ctx, 2);
                    linenoiseAddCompletion(lc, duk_require_string(ctx, -1));
                    duk_pop(ctx);
                }

             next:
                duk_pop(ctx);
            }
            return 0;
        } else {
            if (duk_get_prop(ctx, idx_obj)) {
                duk_to_object(ctx, -1);  /* for properties of plain strings etc */
                duk_replace(ctx, idx_obj);
                p_curr = p + 1;
            } else {
                /* Not found. */
                return 0;
            }
        }
    }

    return 0;
}


static void linenoise_completion(const char *buf, linenoiseCompletions *lc) {
    duk_context *ctx;
    const unsigned char *p_start;
    const unsigned char *p_end;
    const unsigned char *p;

    if (!buf) {
        return;
    }
    ctx = sjs_vm_get_duk_ctx(cli.vm);
    if (!ctx) {
        return;
    }

    p_start = (const unsigned char *) buf;
    p_end = (const unsigned char *) (buf + strlen(buf));
    p = p_end;

    /* Scan backwards for a maximal string which looks like a property
     * chain (e.g. foo.bar.quux).
     */

    while (--p >= p_start) {
        if (p[0] == (unsigned char) '.') {
            if (p <= p_start) {
                break;
            }
            if (!completion_idpart(p[-1])) {
                /* Catches e.g. 'foo..bar' -> we want 'bar' only. */
                break;
            }
        } else if (!completion_idpart(p[0])) {
            break;
        }
    }
    /* 'p' will either be p_start - 1 (ran out of buffer) or point to
     * the first offending character.
     */
    p++;
    if (p < p_start || p >= p_end) {
        return;  /* should never happen, but just in case */
    }

    /* 'p' now points to a string of the form 'foo.bar.quux'.  Look up
     * all the components except the last; treat the last component as
     * a partial name which is used as a filter for the previous full
     * component.  All lookups are from the global object now.
     */

#ifdef SJS_CLI_DEBUG
    fprintf(stderr, "Completion starting point: '%s'\n", p);
    fflush(stderr);
#endif

    duk_push_string(ctx, (const char *) buf);
    duk_push_lstring(ctx, (const char *) p, (duk_size_t) (p_end - p));
    duk_push_pointer(ctx, (void *) lc);

    (void) duk_safe_call(ctx, linenoise_completion_lookup, 3 /*nargs*/, 1 /*nrets*/);
    duk_pop(ctx);
}


static int handle_stdin(void) {
    char *buf;
    size_t bufsz;
    size_t bufoff;
    size_t got;
    int r;

    buf = malloc(SJS_CLI_STDIN_BUF_SIZE);
    if (!buf) {
        goto error;
    }

    bufsz = SJS_CLI_STDIN_BUF_SIZE;
    bufoff = 0;

    /* Read until EOF, avoid fseek/stat because it won't work with stdin. */
    for (;;) {
        size_t avail;

        avail = bufsz - bufoff;
        if (avail < 1024) {
            bufsz = bufsz + SJS_CLI_STDIN_BUF_SIZE;
            buf = realloc(buf, bufsz);
            if (!buf) {
                goto error;
            }
        }

        avail = bufsz - bufoff;
        got = fread((void *) (buf + bufoff), 1, avail, stdin);
        if (got == 0) {
            break;
        }
        bufoff += got;
    }

    r = sjs_vm_eval_code(cli.vm, "stdin", buf, bufoff, NULL, stderr, cli.options.use_strict);

    free(buf);
    buf = NULL;

    if (r == DUK_EXEC_SUCCESS) {
        return 0;
    }

error:
    fprintf(stderr, "error executing <stdin>\n");
    fflush(stderr);
    return -1;
}


static int handle_interactive(void) {
    const char *prompt = "sjs> ";
    const char* default_history_file = "~/.sjs_history";
    duk_context* ctx;
    char* line;
    char* tmp;
    char history_file[4096];
    int use_history;

    ctx = sjs_vm_get_duk_ctx(cli.vm);
    duk_eval_string_noresult(ctx, SJS__CLI_GREET_CODE);

    /* setup history file */
    tmp = getenv("SJS_HISTORY_FILE");
    if (tmp == NULL) {
        tmp = (char*) default_history_file;
    }
    if (sjs_path_expanduser(tmp, history_file, sizeof(history_file)) == 0) {
        linenoiseHistorySetMaxLen(1000);
        linenoiseHistoryLoad(history_file);
        use_history = 1;
    } else {
        use_history = 0;
    }

    linenoiseSetCompletionCallback(linenoise_completion);

    while((line = linenoise(prompt)) != NULL) {
        /* reset sigint signal state */
        cli.got_sigint = 0;

        if (line[0] != '\0') {
            linenoiseHistoryAdd(line);
        }

        sjs_vm_eval_code(cli.vm, "input", line, strlen(line), stdout, stdout, cli.options.use_strict);
        linenoiseFree(line);
    }

    if (use_history) {
        linenoiseHistorySave(history_file);
    }

    return 0;
}


static void handle_sigint(int sig) {
    assert(sig == SIGINT);
    if (cli.got_sigint == 1) {
        exit(EXIT_SUCCESS);
    } else {
        cli.got_sigint = 1;
        fprintf(stdout, "\r\n(^C again to exit)\r\n");
        fflush(stdout);
    }
}


int main(int argc, char *argv[]) {
    int retval = EXIT_SUCCESS;

    memset(&cli, 0, sizeof(cli));

    /* Parse options */
    for (int i = 1; i < argc; i++) {
        char *arg = argv[i];
        assert(arg);
        if (strcmp(arg, "--use_strict") == 0) {
            cli.options.use_strict = 1;
        } else if (strcmp(arg, "-i") == 0) {
            cli.options.interactive = 1;
        } else if (strcmp(arg, "-h") == 0) {
            goto usage;
        } else if (strcmp(arg, "-e") == 0) {
            if (i == argc - 1) {
                retval = EXIT_FAILURE;
                goto usage;
            }
            cli.options.mode = SJS_CLI_EVAL;
            cli.options.data = argv[i + 1];
            break;
        } else if (strlen(arg) > 1 && arg[0] == '-') {
            retval = EXIT_FAILURE;
            goto usage;
        } else if (strcmp(arg, "-") == 0) {
            cli.options.mode = SJS_CLI_STDIN;
            break;
        } else {
            cli.options.mode = SJS_CLI_FILE;
            cli.options.data = arg;
            break;
        }
    }

    if (cli.options.mode == SJS_CLI_REPL) {
        cli.options.interactive = 1;
    }

    /* Create VM */
    cli.vm = sjs_vm_create();
    sjs_vm_setup_args(cli.vm, argc, argv);

    /* run */
    switch (cli.options.mode) {
        case SJS_CLI_FILE:
            if (sjs_vm_eval_file(cli.vm, cli.options.data, NULL, stderr, cli.options.use_strict) != 0) {
                goto error;
            }
            break;
        case SJS_CLI_STDIN:
            if (handle_stdin() != 0) {
                goto error;
            }
            break;
        case SJS_CLI_EVAL:
            if (sjs_vm_eval_code(cli.vm, "eval", cli.options.data, strlen(cli.options.data), NULL, stderr, cli.options.use_strict) != 0) {
                goto error;
            }
            break;
        default:
            break;
    }

    if (!isatty(STDIN_FILENO)) {
        cli.options.interactive = 0;
        if (handle_stdin() != 0) {
            goto error;
        }
    }

    /* Enter interactive mode */
    if (cli.options.interactive) {
        /* setup signal handling */
        signal(SIGPIPE, SIG_IGN);
        signal(SIGINT, handle_sigint);

        if (handle_interactive() != 0) {
            goto error;
        }
    }

    goto cleanup;

error:
    retval = EXIT_FAILURE;

cleanup:
    sjs_vm_destroy(cli.vm);
    exit(retval);

usage:
    fprintf(stderr, "Usage: sjs [options] [ <code> | <file> | - ]\n"
                    "\n"
                    "   -h         show help text\n"
                    "   -i         enter interactive mode after executing argument file(s) / eval code\n"
                    "   -e CODE    evaluate code\n"
                    "\n"
                    "   --use_strict    evaluate the code in strict mode\n"
                    "\n"
                    "If <file> is omitted, interactive mode is started automatically.\n");
    fflush(stderr);
    exit(retval);
}


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/stat.h>
#include <unistd.h>

#include "duktape.h"
#include "linenoise.h"
#include "vm.h"


#define GREET_CODE                                                   \
    "print('Welcome to Skookum JS');"                                \
    "var dukVersion = '" DUK_GIT_DESCRIBE "';"                       \
    "var dukCommit = '" DUK_GIT_COMMIT "'.substr(0, 7);"             \
    "print('[Duktape ' + dukVersion + ' (' + dukCommit + ')]');"     \
    "delete dukVersion;"                                             \
    "delete dukCommit;"


static int get_stack_raw(duk_context *ctx) {
    if (!duk_is_object(ctx, -1)) {
        return 1;
    }

    if (!duk_has_prop_string(ctx, -1, "stack")) {
        return 1;
    }

    if (!duk_is_error(ctx, -1)) {
        /* Not an Error instance, don't read "stack". */
        return 1;
    }

    duk_get_prop_string(ctx, -1, "stack");  /* caller coerces */
    duk_remove(ctx, -2);
    return 1;
}


/* Print error to stderr and pop error. */
static void print_pop_error(duk_context *ctx, FILE *f) {
    /* Print error objects with a stack trace specially.
     * Note that getting the stack trace may throw an error
     * so this also needs to be safe call wrapped.
     */
    (void) duk_safe_call(ctx, get_stack_raw, 1 /*nargs*/, 1 /*nrets*/);
    fprintf(f, "%s\n", duk_safe_to_string(ctx, -1));
    fflush(f);
    duk_pop(ctx);
}


static int wrapped_compile_execute(duk_context *ctx) {
    const char *src_data;
    duk_size_t src_len;
    duk_bool_t write_output;

    /* [ ... write_output src_data src_len filename ] */

    write_output = duk_require_boolean(ctx, -4);
    src_data = (const char *) duk_require_pointer(ctx, -3);
    src_len = (duk_size_t) duk_require_uint(ctx, -2);
    duk_compile_lstring_filename(ctx, 0, src_data, src_len);

    /* [ ... write_output src_data src_len function ] */

    duk_push_global_object(ctx);  /* 'this' binding */
    duk_call_method(ctx, 0);

    if (write_output) {
        fprintf(stdout, "= %s\n", duk_to_string(ctx, -1));
        fflush(stdout);
    }

    return 0;  /* duk_safe_call() cleans up */
}


static int handle_stdin(duk_context *ctx) {
    char *buf = NULL;
    size_t bufsz;
    size_t bufoff;
    size_t got;
    int rc;
    int retval = -1;

    buf = (char *) malloc(1024);
    if (!buf) {
        goto error;
    }
    bufsz = 1024;
    bufoff = 0;

    /* Read until EOF, avoid fseek/stat because it won't work with stdin. */
    for (;;) {
        size_t avail;

        avail = bufsz - bufoff;
        if (avail < 1024) {
            size_t newsz;
            newsz = bufsz + (bufsz >> 2) + 1024;  /* +25% and some extra */
            buf = (char *) realloc(buf, newsz);
            if (!buf) {
                goto error;
            }
            bufsz = newsz;
        }

        avail = bufsz - bufoff;
        got = fread((void *) (buf + bufoff), (size_t) 1, avail, stdin);
        if (got == 0) {
            break;
        }
        bufoff += got;
    }

    duk_push_false(ctx);
    duk_push_pointer(ctx, (void *) buf);
    duk_push_uint(ctx, (duk_uint_t) bufoff);
    duk_push_string(ctx, "stdin");

    rc = duk_safe_call(ctx, wrapped_compile_execute, 4 /*nargs*/, 1 /*nret*/);

    free(buf);
    buf = NULL;

    if (rc != DUK_EXEC_SUCCESS) {
        print_pop_error(ctx, stderr);
        goto error;
    } else {
        duk_pop(ctx);
        retval = 0;
    }
    /* fall thru */

cleanup:
    if (buf) {
        free(buf);
    }
    return retval;

error:
    fprintf(stderr, "error executing <stdin>\n");
    fflush(stderr);
    goto cleanup;
}


static int handle_eval(duk_context *ctx, char *code) {
    int rc;
    int retval = -1;

    duk_push_false(ctx);
    duk_push_pointer(ctx, (void *) code);
    duk_push_uint(ctx, (duk_uint_t) strlen(code));
    duk_push_string(ctx, "eval");

    rc = duk_safe_call(ctx, wrapped_compile_execute, 4 /*nargs*/, 1 /*nret*/);

    if (rc != DUK_EXEC_SUCCESS) {
        print_pop_error(ctx, stderr);
    } else {
        duk_pop(ctx);
        retval = 0;
    }

    return retval;
}


static int handle_interactive(duk_context *ctx) {
    const char *prompt = "sjs> ";
    char* line;
    int rc;

    duk_eval_string(ctx, GREET_CODE);
    duk_pop(ctx);

    while((line = linenoise(prompt)) != NULL) {
        if (line[0] != '\0') {
            linenoiseHistoryAdd(line);
        }

        duk_push_true(ctx);
        duk_push_pointer(ctx, (void *) line);
        duk_push_uint(ctx, (duk_uint_t) strlen(line));
        duk_push_string(ctx, "input");

        rc = duk_safe_call(ctx, wrapped_compile_execute, 4 /*nargs*/, 1 /*nret*/);

        free(line);

        if (rc != DUK_EXEC_SUCCESS) {
            /* in interactive mode, write to stdout */
            print_pop_error(ctx, stdout);
        } else {
            duk_pop(ctx);
        }
    }

    return 0;
}


static int handle_file(duk_context *ctx, char* filename) {
    if (duk_peval_file(ctx, filename) != 0) {
        print_pop_error(ctx, stderr);
        return -1;
    } else {
        duk_pop(ctx);
        return 0;
    }
}


int main(int argc, char *argv[]) {
    sjs_vm_t* vm = NULL;
    duk_context *ctx = NULL;
    int retval = 0;
    int have_files = 0;
    int have_eval = 0;
    int interactive = 0;
    int run_stdin = 0;
    int i;

    /* Signal handling setup */
    signal(SIGPIPE, SIG_IGN);

    /* Parse options */
    for (i = 1; i < argc; i++) {
            char *arg = argv[i];
            if (!arg) {
                    goto usage;
            }
            if (strcmp(arg, "-i") == 0) {
                    interactive = 1;
            } else if (strcmp(arg, "-e") == 0) {
                    have_eval = 1;
                    if (i == argc - 1) {
                            goto usage;
                    }
                    i++;  /* skip code */
            } else if (strlen(arg) > 1 && arg[0] == '-') {
                    goto usage;
            } else if (strlen(arg) == 1 && arg[0] == '-') {
                    run_stdin = 1;
            } else {
                    have_files = 1;
            }
    }

    if (!have_files && !have_eval && !run_stdin) {
        interactive = 1;
    }

    /* Create VM */
    vm = sjs_vm_create();
    ctx = vm->ctx;

    /* Execute any argument file(s) */
    for (i = 1; i < argc; i++) {
        char *arg = argv[i];
        if (!arg) {
            continue;
        } else if (strlen(arg) == 2 && strcmp(arg, "-e") == 0) {
            /* Here we know the eval arg exists but check anyway */
            if (i == argc - 1) {
                retval = 1;
                goto cleanup;
            }
            if (handle_eval(ctx, argv[i + 1]) != 0) {
                retval = 1;
                goto cleanup;
            }
            i++;  /* skip code */
            continue;
        } else if (strlen(arg) > 1 && arg[0] == '-') {
            continue;
        } else if (strlen(arg) == 1 && arg[0] == '-') {
            if (handle_stdin(ctx) != 0) {
                retval = 1;
                goto cleanup;
            }
        } else {
            if (handle_file(ctx, arg) != 0) {
                retval = 1;
                goto cleanup;
            }
        }
    }

    if (!isatty(STDIN_FILENO)) {
        interactive = 0;
        if (handle_stdin(ctx) != 0) {
            retval = 1;
            goto cleanup;
        }
    }

    /* Enter interactive mode, maybe */
    if (interactive) {
        if (handle_interactive(ctx) != 0) {
            retval = 1;
            goto cleanup;
        }
    }

cleanup:
    sjs_vm_destroy(vm);
    vm = NULL;
    ctx = NULL;
    return retval;

usage:
    fprintf(stderr, "Usage: duk [options] [<filenames> | -]\n"
                    "\n"
                    "   -i         enter interactive mode after executing argument file(s) / eval code\n"
                    "   -e CODE    evaluate code\n"
                    "\n"
                    "If <filename> is omitted, interactive mode is started automatically.\n");
    fflush(stderr);
    exit(1);
}

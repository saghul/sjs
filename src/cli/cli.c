
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


static int handle_stdin(sjs_vm_t* vm) {
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

    r = sjs_vm_eval_code(vm, "stdin", buf, bufoff, NULL, stderr, cli.options.use_strict);

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


static int handle_interactive(sjs_vm_t* vm) {
    const char *prompt = "sjs> ";
    duk_context* ctx;
    char* line;
    char history_file[4096];
    int use_history;

    ctx = sjs_vm_get_duk_ctx(vm);
    duk_eval_string_noresult(ctx, SJS__CLI_GREET_CODE);

    /* setup history file
     * TODO: make configurable?
     */
    if (sjs_path_normalize("~", history_file, sizeof(history_file)) == 0) {
        strcat(history_file, "/.sjs_history");
        linenoiseHistorySetMaxLen(1000);
        linenoiseHistoryLoad(history_file);
        use_history = 1;
    } else {
        use_history = 0;
    }

    while((line = linenoise(prompt)) != NULL) {
        /* reset sigint signal state */
        cli.got_sigint = 0;

        if (line[0] != '\0') {
            linenoiseHistoryAdd(line);
        }

        sjs_vm_eval_code(vm, "input", line, strlen(line), stdout, stdout, cli.options.use_strict);
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
                retval = EXIT_FAILURE;
                goto cleanup;
            }
            break;
        case SJS_CLI_STDIN:
            if (handle_stdin(cli.vm) != 0) {
                retval = EXIT_FAILURE;
                goto cleanup;
            }
            break;
        case SJS_CLI_EVAL:
            if (sjs_vm_eval_code(cli.vm, "eval", cli.options.data, strlen(cli.options.data), NULL, stderr, cli.options.use_strict) != 0) {
                retval = EXIT_FAILURE;
                goto cleanup;
            }
            break;
        default:
            break;
    }

    if (!isatty(STDIN_FILENO)) {
        cli.options.interactive = 0;
        if (handle_stdin(cli.vm) != 0) {
            retval = EXIT_FAILURE;
            goto cleanup;
        }
    }

    /* Enter interactive mode */
    if (cli.options.interactive) {
        /* setup signal handling */
        signal(SIGPIPE, SIG_IGN);
        signal(SIGINT, handle_sigint);

        if (handle_interactive(cli.vm) != 0) {
            retval = EXIT_FAILURE;
            goto cleanup;
        }
    }

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

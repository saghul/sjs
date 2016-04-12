
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/stat.h>
#include <unistd.h>

#include "linenoise.h"
#include "sjs/sjs.h"


/* generated from src/cli/greet.js */
static const char SJS__CLI_GREET_CODE[] = {
    40, 102, 117, 110, 99, 116, 105, 111, 110, 40, 41, 32, 123, 10, 32, 32,
    32, 32, 112, 114, 105, 110, 116, 40, 39, 83, 107, 111, 111, 107, 117, 109,
    32, 74, 83, 32, 39, 32, 43, 32, 115, 121, 115, 116, 101, 109, 46, 118, 101,
    114, 115, 105, 111, 110, 115, 46, 115, 106, 115, 32, 43, 32, 39, 32, 111,
    110, 32, 39, 32, 43, 32, 115, 121, 115, 116, 101, 109, 46, 112, 108, 97,
    116, 102, 111, 114, 109, 32, 43, 32, 39, 32, 40, 39, 32, 43, 32, 115, 121,
    115, 116, 101, 109, 46, 97, 114, 99, 104, 32, 43, 32, 39, 41, 39, 41, 59,
    10, 32, 32, 32, 32, 112, 114, 105, 110, 116, 40, 39, 91, 68, 117, 107, 116,
    97, 112, 101, 32, 39, 32, 43, 32, 115, 121, 115, 116, 101, 109, 46, 118,
    101, 114, 115, 105, 111, 110, 115, 46, 100, 117, 107, 116, 97, 112, 101,
    32, 43, 32, 39, 32, 40, 39, 32, 43, 32, 115, 121, 115, 116, 101, 109, 46,
    118, 101, 114, 115, 105, 111, 110, 115, 46, 100, 117, 107, 116, 97, 112,
    101, 67, 111, 109, 109, 105, 116, 32, 43, 32, 39, 41, 93, 39, 41, 59, 10,
    125, 41, 40, 41, 59, 10, 0
};

#define SJS_CLI_STDIN_BUF_SIZE    65536


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
    char *buf;
    size_t bufsz;
    size_t bufoff;
    size_t got;
    int rc;

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
        return 0;
    }

error:
    fprintf(stderr, "error executing <stdin>\n");
    fflush(stderr);
    return -1;
}


static int handle_eval(duk_context *ctx, char *code) {
    int rc;

    duk_push_false(ctx);
    duk_push_pointer(ctx, (void *) code);
    duk_push_uint(ctx, (duk_uint_t) strlen(code));
    duk_push_string(ctx, "eval");

    rc = duk_safe_call(ctx, wrapped_compile_execute, 4 /*nargs*/, 1 /*nret*/);
    if (rc != DUK_EXEC_SUCCESS) {
        print_pop_error(ctx, stderr);
        return -1;
    } else {
        duk_pop(ctx);
        return 0;
    }
}


static int handle_interactive(duk_context *ctx) {
    const char *prompt = "sjs> ";
    char* line;
    int rc;

    duk_eval_string_noresult(ctx, SJS__CLI_GREET_CODE);

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
    char* run_file = NULL;
    char* eval_code = NULL;
    int interactive = 0;
    int run_stdin = 0;
    int retval = 0;

    /* Signal handling setup */
    signal(SIGPIPE, SIG_IGN);

    /* Parse options */
    for (int i = 1; i < argc; i++) {
        char *arg = argv[i];
        assert(arg);
        if (strcmp(arg, "-i") == 0) {
            interactive = 1;
        } else if (strcmp(arg, "-h") == 0) {
            goto usage;
        } else if (strcmp(arg, "-e") == 0) {
            if (i == argc - 1) {
                retval = 1;
                goto usage;
            }
            eval_code = argv[i + 1];
            break;
        } else if (strlen(arg) > 1 && arg[0] == '-') {
            retval = 1;
            goto usage;
        } else if (strcmp(arg, "-") == 0) {
            run_stdin = 1;
            break;
        } else {
            run_file = arg;
            break;
        }
    }

    if (!run_file && !eval_code && !run_stdin) {
        interactive = 1;
    }

    /* Create VM */
    vm = sjs_vm_create();
    sjs_vm_setup_args(vm, argc, argv);
    ctx = sjs_vm_get_duk_ctx(vm);

    /* run */
    if (run_file) {
        if (handle_file(ctx, run_file) != 0) {
            retval = 1;
            goto cleanup;
        }
    } else if (run_stdin) {
        if (handle_stdin(ctx) != 0) {
            retval = 1;
            goto cleanup;
        }
    } else if (eval_code) {
        if (handle_eval(ctx, eval_code) != 0) {
            retval = 1;
            goto cleanup;
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
    fprintf(stderr, "Usage: duk [options] [ <code> | <file> | - ]\n"
                    "\n"
                    "   -h         show help text\n"
                    "   -i         enter interactive mode after executing argument file(s) / eval code\n"
                    "   -e CODE    evaluate code\n"
                    "\n"
                    "If <file> is omitted, interactive mode is started automatically.\n");
    fflush(stderr);
    exit(retval);
}

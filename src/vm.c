
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef __APPLE__
# include <crt_externs.h>
# define environ (*_NSGetEnviron())
#else
extern char** environ;
#endif

#include "duktape.h"
#include "vm.h"
#include "version.h"
#include "internal.h"
#include "es6shim.h"


struct sjs_vm_t {
    duk_context* ctx;
};


/* generated from src/bootstrap.js */
static const char SJS__BOOTSTRAP_CODE[] = {
    0,
};


static const char* default_search_paths[] = {
    ".",
    "./modules",
    "/usr/lib/sjs/modules",
    "/usr/local/lib/sjs/modules",
    "~/.local/sjs/modules",
    NULL
};


static void sjs__duk_fatal_handler(duk_context *ctx, duk_errcode_t code, const char *msg) {
    fprintf(stderr, "FATAL %ld: %s\n", (long) code, (const char *) (msg ? msg : "null"));
    fflush(stderr);
    abort();
}


static duk_ret_t sjs__exit(duk_context* ctx) {
    int code = duk_require_int(ctx, 0);
    /* TODO: properly tear down the vm */
    exit(code);
    return -42;    /* control never returns here */
}


static void sjs__setup_system_module(sjs_vm_t* vm) {
    duk_context* ctx = vm->ctx;

    /* create system module */
    duk_push_object(ctx);
    duk_put_global_string(ctx, "system");

    duk_get_global_string(ctx, "system");
    /* -> [ ... system ] */

    /* system.versions */
    {
        duk_push_object(ctx);
        /* -> [ ... system obj ] */

        duk_push_string(ctx, DUK_GIT_DESCRIBE);
        duk_put_prop_string(ctx, -2, "duktape");
        duk_push_lstring(ctx, DUK_GIT_COMMIT, 7);
        duk_put_prop_string(ctx, -2, "duktapeCommit");
        duk_push_string(ctx, sjs_version());
        duk_put_prop_string(ctx, -2, "sjs");
        /* -> [ ... system obj ] */

        duk_put_prop_string(ctx, -2, "versions");
        /* -> [ ... system ] */
    }

    /* system.env */
    {
        duk_push_object(ctx);
        /* -> [ ... system obj ] */

        for (int i = 0; environ[i]; i++) {
            const char* e = environ[i];
            const char* ptr = strrchr(e, '=');
            const char* key;
            const char* value;
            if (!ptr) {
                continue;
            }
            key = e;
            value = ptr + 1;

            duk_push_lstring(ctx, key, (duk_size_t)(ptr - key));
            duk_push_string(ctx, value);
            duk_put_prop(ctx, -3);
        }
        /* -> [ ... system obj ] */

        duk_put_prop_string(ctx, -2, "env");
        /* -> [ ... system ] */
    }

    /* system.path */
    {
        int i = 0;
        duk_push_array(ctx);
        /* -> [ ... system array ] */

        /* path specified by env variable */
        {
            char* sjs_path = getenv("SJS_PATH");
            if (sjs_path) {
                char* token;
                char* saveptr;
                token = strtok_r(sjs_path, ":", &saveptr);
                for (;;) {
                    if (!token) {
                        break;
                    }
                    duk_push_string(ctx, token);
                    duk_put_prop_index(ctx, -2, i);
                    i++;
                    token = strtok_r(NULL, ":", &saveptr);
                }
            }
        }

        /* default search paths */
        {
            for (int j = 0; default_search_paths[j]; j++) {
                duk_push_string(ctx, default_search_paths[j]);
                duk_put_prop_index(ctx, -2, i);
                i++;
            }
        }

        duk_put_prop_string(ctx, -2, "path");
        /* -> [ ... system ] */
    }

    /* system.arch */
    {
        duk_push_string(ctx, DUK_USE_ARCH_STRING);
        duk_put_prop_string(ctx, -2, "arch");
    }

    /* system.platform */
    {
        duk_push_string(ctx, DUK_USE_OS_STRING);
        duk_put_prop_string(ctx, -2, "platform");
    }

    /* system.executable */
    {
        char buf[8192] = {0};
        sjs__executable(buf, sizeof(buf));
        duk_push_string(ctx, buf);
        duk_put_prop_string(ctx, -2, "executable");
    }

    /* system.exit(x) */
    {
        duk_push_c_function(ctx, sjs__exit, 1 /* nargs */);
        duk_put_prop_string(ctx, -2, "exit");
    }

    duk_pop(ctx);
}


static void sjs__setup_modsearch(sjs_vm_t* vm) {
    duk_context* ctx = vm->ctx;

    duk_get_global_string(ctx, "Duktape");
    duk_push_c_function(ctx, sjs__modsearch, 4 /* nargs */);
    duk_put_prop_string(ctx, -2, "modSearch");
    duk_pop(ctx);
}


static void sjs__setup_global_module(sjs_vm_t* vm) {
    duk_context* ctx = vm->ctx;

    duk_push_global_object(ctx);
    duk_put_global_string(ctx, "global");
}


static int sjs__compile_execute(duk_context *ctx) {
    const char *code;
    duk_size_t len;

    /* [ ... code len filename ] */

    code = (const char *) duk_require_pointer(ctx, -3);
    len = (duk_size_t) duk_require_uint(ctx, -2);
    duk_compile_lstring_filename(ctx, 0, code, len);

    /* [ ... code len function ] */

    duk_push_global_object(ctx);  /* 'this' binding */
    duk_call_method(ctx, 0);

    return 0;  /* duk_safe_call() cleans up */
}


static int sjs__get_error_stack(duk_context *ctx) {
    if (!duk_is_object(ctx, -1)) {
        return 1;
    }

    if (!duk_has_prop_string(ctx, -1, "stack")) {
        return 1;
    }

    if (!duk_is_error(ctx, -1)) {
        /* Not an Error instance, don't read "stack" */
        return 1;
    }

    duk_get_prop_string(ctx, -1, "stack");  /* caller coerces */
    duk_remove(ctx, -2);
    return 1;
}


DUK_EXTERNAL sjs_vm_t* sjs_vm_create(void) {
    sjs_vm_t* vm;
    vm = calloc(1, sizeof(*vm));
    assert(vm);

    vm->ctx = duk_create_heap(NULL,                     /* alloc function */
                              NULL,                     /* realloc function */
                              NULL,                     /* free function */
                              NULL,                     /* user data */
                              sjs__duk_fatal_handler    /* fatal error handler */
                             );
    assert(vm->ctx != NULL);

    /* setup builtin modules */
    sjs__setup_system_module(vm);
    sjs__setup_global_module(vm);
    sjs__setup_modsearch(vm);

    /* setup es6 shim */
    duk_eval_string_noresult(vm->ctx, (char*) sjs__es6shim_src);

    /* run bootstrap code */
    duk_eval_string_noresult(vm->ctx, SJS__BOOTSTRAP_CODE);

    return vm;
}


DUK_EXTERNAL void sjs_vm_destroy(sjs_vm_t* vm) {
    assert(vm);

    duk_destroy_heap(vm->ctx);
    vm->ctx = NULL;
    free(vm);
}


DUK_EXTERNAL void sjs_vm_setup_args(sjs_vm_t* vm, int argc, char* argv[]) {
    assert(vm);

    duk_context* ctx = vm->ctx;

    duk_get_global_string(ctx, "system");

    /* system.argv */
    {
        duk_push_array(ctx);
        /* -> [ ... system array ] */

        for (int i = 0; i < argc; i++) {
            duk_push_string(ctx, argv[i]);
            duk_put_prop_index(ctx, -2, i);
        }

        duk_put_prop_string(ctx, -2, "argv");
        /* -> [ ... system ] */
    }

    duk_pop(ctx);
}


DUK_EXTERNAL int sjs_vm_eval_code(const sjs_vm_t* vm, const char* filename, const char* code, size_t len, FILE* foutput, FILE* ferror) {
    int r;

    assert(vm);
    duk_context* ctx = vm->ctx;

    duk_push_pointer(ctx, (void *) code);
    duk_push_uint(ctx, len);
    duk_push_string(ctx, filename);

    r = duk_safe_call(ctx, sjs__compile_execute, 3 /*nargs*/, 1 /*nret*/);
    if (r != DUK_EXEC_SUCCESS) {
        if (ferror) {
            duk_safe_call(ctx, sjs__get_error_stack, 1 /*nargs*/, 1 /*nrets*/);
            fprintf(ferror, "%s\n", duk_safe_to_string(ctx, -1));
            fflush(ferror);
        }
    } else {
        if (foutput) {
            fprintf(foutput, "= %s\n", duk_safe_to_string(ctx, -1));
            fflush(foutput);
        }
    }

    duk_pop(ctx);
    return r;
}


/* TODO: refactor? read the file and use the eval code function */
DUK_EXTERNAL int sjs_vm_eval_file(const sjs_vm_t* vm, const char* filename, FILE* foutput, FILE* ferror) {
    int r;

    assert(vm);
    duk_context* ctx = vm->ctx;

    r = duk_peval_file(ctx, filename);
    if (r != DUK_EXEC_SUCCESS) {
        if (ferror) {
            duk_safe_call(ctx, sjs__get_error_stack, 1 /*nargs*/, 1 /*nrets*/);
            fprintf(ferror, "%s\n", duk_safe_to_string(ctx, -1));
            fflush(ferror);
        }
    } else {
        if (foutput) {
            fprintf(foutput, "= %s\n", duk_safe_to_string(ctx, -1));
            fflush(foutput);
        }
    }

    duk_pop(ctx);
    return r;
}


DUK_EXTERNAL duk_context* sjs_vm_get_duk_ctx(sjs_vm_t* vm) {
    assert(vm);
    return vm->ctx;
}


DUK_EXTERNAL int sjs_path_normalize(const char* path, char* normalized_path, size_t normalized_path_len) {
    return sjs__path_normalize(path, normalized_path, normalized_path_len);
}

DUK_EXTERNAL uint64_t sjs_time_hrtime(void) {
    return sjs__hrtime();
}

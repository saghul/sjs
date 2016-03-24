
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


struct sjs_vm_t {
    duk_context* ctx;
};


/* generated from src/bootstrap.js */
static const char SJS__BOOTSTRAP_CODE[] = {
    40, 102, 117, 110, 99, 116, 105, 111, 110, 40, 41, 32, 123, 10, 32,
    32, 32, 32, 47, 47, 32, 116, 104, 101, 32, 95, 101, 115, 54, 32,
    109, 111, 100, 117, 108, 101, 32, 111, 112, 101, 114, 97, 116, 101,
    115, 32, 111, 110, 32, 103, 108, 111, 98, 97, 108, 115, 10, 32, 32,
    32, 32, 114, 101, 113, 117, 105, 114, 101, 40, 39, 95, 101, 115, 54,
    39, 41, 59, 10, 125, 41, 40, 41, 59, 10, 0
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


DUK_EXTERNAL sjs_vm_t* sjs_vm_create(void) {
    sjs_vm_t* vm;
    vm = calloc(1, sizeof(*vm));
    assert(vm);

    vm->ctx = duk_create_heap(NULL,                     /* alloc function */
                              NULL,                     /* realloc function */
                              NULL,                     /* free function */
                              (void*) vm,               /* user data */
                              sjs__duk_fatal_handler    /* fatal error handler */
                             );
    assert(vm->ctx != NULL);

    /* setup builtin modules */
    sjs__setup_system_module(vm);
    sjs__setup_global_module(vm);
    sjs__setup_modsearch(vm);

    /* run bootstrap code*/
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

    /* system.exe */
    {
        char* executable = realpath(argv[0], NULL);
        assert(executable);
        duk_push_string(ctx,executable);
        /* -> [ ... global system string ] */

        duk_put_prop_string(ctx, -2, "exe");
        /* -> [ ... global system ] */
    }

    duk_pop(ctx);
}


DUK_EXTERNAL duk_context* sjs_vm_get_duk_ctx(sjs_vm_t* vm) {
    assert(vm);
    return vm->ctx;
}


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


struct sjs_vm_t {
    duk_context* ctx;
};


static void sjs__duk_fatal_handler(duk_context *ctx, duk_errcode_t code, const char *msg) {
    fprintf(stderr, "FATAL %ld: %s\n", (long) code, (const char *) (msg ? msg : "null"));
    fflush(stderr);
    abort();
}


static void sjs__create_system_module(sjs_vm_t* vm) {
    duk_context* ctx = vm->ctx;

    duk_push_global_object(ctx);
    duk_push_object(ctx);
    /* -> [ ... global obj ] */

    duk_put_prop_string(ctx, -2, "system");
    /* -> [ ... global ] */

    duk_pop(ctx);
}


static void sjs__setup_system_module(sjs_vm_t* vm) {
    duk_context* ctx = vm->ctx;

    duk_push_global_object(ctx);
    duk_get_prop_string(ctx, -1, "system");
    /* -> [ ... global system ] */

    /* add the 'versions' object */

    duk_push_object(ctx);
    /* -> [ ... global system obj ] */

    duk_push_string(ctx, DUK_GIT_DESCRIBE);
    duk_put_prop_string(ctx, -2, "duktape");
    /* -> [ ... global system obj ] */

    duk_put_prop_string(ctx, -2, "versions");
    /* -> [ ... global system ] */

    /* add the env object */

    duk_push_object(ctx);
    /* -> [ ... global system obj ] */

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
    /* -> [ ... global system obj ] */

    duk_put_prop_string(ctx, -2, "env");
    /* -> [ ... global system ] */

    duk_pop(ctx);
    duk_pop(ctx);
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

    sjs__create_system_module(vm);
    sjs__setup_system_module(vm);

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

    duk_push_global_object(ctx);
    duk_get_prop_string(ctx, -1, "system");
    /* -> [ ... global system ] */

    duk_push_array(ctx);
    /* -> [ ... global system array ] */

    for (int i = 0; i < argc; i++) {
        duk_push_string(ctx, argv[i]);
        duk_put_prop_index(ctx, -2, i);
    }

    duk_put_prop_string(ctx, -2, "argv");
    /* -> [ ... global system ] */

    duk_pop(ctx);
    duk_pop(ctx);
}


DUK_EXTERNAL duk_context* sjs_vm_get_duk_ctx(sjs_vm_t* vm) {
    assert(vm);
    return vm->ctx;
}

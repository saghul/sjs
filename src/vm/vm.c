
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "duktape.h"
#include "vm.h"


static void sjs__duk_fatal_handler(duk_context *ctx, duk_errcode_t code, const char *msg) {
    fprintf(stderr, "FATAL %ld: %s\n", (long) code, (const char *) (msg ? msg : "null"));
    fflush(stderr);
    abort();
}


static void sjs__setup_system_module(sjs_vm_t* vm) {
    duk_context* ctx = vm->ctx;

    duk_push_global_object(ctx);
    duk_push_object(ctx);
    duk_push_object(ctx);
    /* -> [ ... global obj obj ] */

    duk_push_string(ctx, DUK_GIT_DESCRIBE);
    /* -> [ ... global obj obj val ] */

    duk_put_prop_string(ctx, -2, "duktape");
    /* -> [ ... global obj obj ] */

    duk_put_prop_string(ctx, -2, "versions");
    /* -> [ ... global obj ] */

    duk_put_prop_string(ctx, -2, "system");
    /* -> [ ... global ] */

    duk_pop(ctx);
}


DUK_EXTERNAL sjs_vm_t* sjs_vm_create(void) {
    sjs_vm_t* vm;
    vm = calloc(1, sizeof(*vm));
    assert(vm != NULL);

    vm->ctx = duk_create_heap(NULL,                     /* alloc function */
			      NULL,                     /* realloc function */
			      NULL,                     /* free function */
			      (void*) vm,               /* user data */
			      sjs__duk_fatal_handler    /* fatal error handler */
			     );
    assert(vm->ctx != NULL);

    sjs__setup_system_module(vm);

    return vm;
}


DUK_EXTERNAL void sjs_vm_destroy(sjs_vm_t* vm) {
    if (vm != NULL) {
	duk_destroy_heap(vm->ctx);
	vm->ctx = NULL;
	free(vm);
    }
}


DUK_EXTERNAL void sjs_vm_setup_args(sjs_vm_t* vm, int argc, char* argv[]) {
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

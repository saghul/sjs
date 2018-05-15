
#include "binding.h"


static duk_bool_t get_cached_binding(duk_context *ctx, const char *id) {
	duk_push_global_stash(ctx);
	(void) duk_get_prop_string(ctx, -1, "\xff" "bindingCache");
	if (duk_get_prop_string(ctx, -1, id)) {
		duk_remove(ctx, -2);
		duk_remove(ctx, -2);
		return 1;
	} else {
		duk_pop_3(ctx);
		return 0;
	}
}


static duk_ret_t binding_func(duk_context *ctx) {
    const char* id;

	id = duk_require_string(ctx, 0);
	if (get_cached_binding(ctx, id)) {
	    /* use the cached module */
	    return 1;
	}

    return duk_error(ctx, DUK_ERR_ERROR, "Binding could not be loaded: %s", id);
}


void sjs__setup_binding(duk_context* ctx) {
    /* system.binding */
    duk_get_global_string(ctx, "_system");
    duk_push_string(ctx, "binding");
	duk_push_c_function(ctx, binding_func, 1);
    duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_SET_WRITABLE | DUK_DEFPROP_SET_CONFIGURABLE);
	duk_pop(ctx);

	/* Initialize the bindings cache to a fresh object. */
	duk_push_global_stash(ctx);
	duk_push_object(ctx);
	duk_put_prop_string(ctx, -2, "\xff" "bindingCache");
	duk_pop(ctx);

	/* Initialize bindings. */
    sjs__binding_errno_init(ctx);
    sjs__binding_gai_init(ctx);
    sjs__binding_hash_init(ctx);
    sjs__binding_io_init(ctx);
    sjs__binding_os_init(ctx);
    sjs__binding_path_init(ctx);
    sjs__binding_poll_init(ctx);
    sjs__binding_pwd_init(ctx);
    sjs__binding_random_init(ctx);
    sjs__binding_select_init(ctx);
    sjs__binding_socket_init(ctx);
}


void sjs__register_binding(duk_context* ctx,
                           const char* id,
                           const duk_function_list_entry functions[],
                           const duk_number_list_entry constants[]) {
    /* setup binding object */
    duk_push_object(ctx);
    if (functions) {
        duk_put_function_list(ctx, -1, functions);
    }
    if (constants) {
        duk_push_object(ctx);
        duk_put_number_list(ctx, -1, constants);
        duk_put_prop_string(ctx, -2, "c");
    }

	/* [ ... binding ] */

	duk_push_global_stash(ctx);
	(void) duk_get_prop_string(ctx, -1, "\xff" "bindingCache");
	duk_dup(ctx, -3);

	/* [ ... binding stash cache binding ] */

	duk_put_prop_string(ctx, -2, id);

	/* [ ... binding stash cache ] */

	duk_pop_3(ctx);  /* cleanup */
}


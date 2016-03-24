
#include <libgen.h>

#include <sjs/sjs.h>


static duk_ret_t path_basename(duk_context* ctx) {
    const char* path;
    char* rpath;

    path = duk_require_string(ctx, 0);
    rpath = basename((char*) path);
    if (!rpath) {
        /* TODO: throw instead? */
        duk_push_undefined(ctx);
    } else {
        duk_push_string(ctx, rpath);
    }
    return 1;
}


static duk_ret_t path_dirname(duk_context* ctx) {
    const char* path;
    char* rpath;

    path = duk_require_string(ctx, 0);
    rpath = dirname((char*) path);
    if (!rpath) {
        /* TODO: throw instead? */
        duk_push_undefined(ctx);
    } else {
        duk_push_string(ctx, rpath);
    }
    return 1;
}


static const duk_function_list_entry module_funcs[] = {
    { "basename", path_basename, 1 /*nargs*/ },
    { "dirname", path_dirname, 1 /*nargs*/ },
    { NULL, NULL, 0 }
};


DUK_EXTERNAL duk_ret_t sjs_mod_init(duk_context* ctx) {
    duk_push_object(ctx);
    duk_put_function_list(ctx, -1, module_funcs);
    return 1;
}

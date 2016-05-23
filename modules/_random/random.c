
#include <assert.h>
/*#include <ctype.h>*/
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include <sjs/sjs.h>
#include "mt19937ar.h"


static duk_ret_t random_alloc(duk_context* ctx) {
    mt_t* state;

    state = malloc(sizeof(*state));
    if (state == NULL) {
        SJS_THROW_ERRNO_ERROR2(ENOMEM);
        return -42;    /* control never returns here */
    }

    duk_push_pointer(ctx, (void*) state);
    return 1;
}


static duk_ret_t random_dealloc(duk_context* ctx) {
    mt_t* state;

    state = duk_require_pointer(ctx, 0);
    free(state);

    duk_push_undefined(ctx);
    return 1;
}


static duk_ret_t random_seed(duk_context* ctx) {
    mt_t* state;

    state = duk_require_pointer(ctx, 0);

    if (duk_is_array(ctx, 1)) {
        size_t i, len;
        len = duk_get_length(ctx, 1);
        assert(len > 0);
        uint32_t seed[len];
        for (i = 0; i< len; i++) {
            duk_get_prop_index(ctx, 1, i);
            seed[i] = duk_require_int(ctx, -1);
            duk_pop(ctx);
        }
        mt_init_by_array(state, seed, len);
    } else {
        uint32_t seed;
        seed = duk_require_uint(ctx, 1);
        mt_init(state, seed);
    }

    return 1;
}


static duk_ret_t random_random(duk_context* ctx) {
    mt_t* state;
    double d;

    state = duk_require_pointer(ctx, 0);
    d = mt_genrand_res53(state);

    duk_push_number(ctx, d);
    return 1;
}


static const duk_function_list_entry module_funcs[] = {
    { "alloc", random_alloc, 0 /*nargs*/ },
    { "dealloc", random_dealloc, 1 /*nargs*/ },
    { "seed", random_seed, 2 /*nargs*/ },
    { "random", random_random, 1 /*nargs*/ },
    { NULL, NULL, 0 }
};


DUK_EXTERNAL duk_ret_t sjs_mod_init(duk_context* ctx) {
    duk_push_object(ctx);
    duk_put_function_list(ctx, -1, module_funcs);
    return 1;
}

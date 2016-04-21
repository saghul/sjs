
#include <errno.h>
#include <sys/select.h>
#include <sys/time.h>

#include <sjs/sjs.h>


static duk_ret_t time_time(duk_context* ctx) {
    int r;
    struct timeval tv;

    r = gettimeofday(&tv, NULL);
    if (r < 0) {
        duk_push_error_object(ctx, DUK_ERR_ERROR, "[errno %d] %s", (errno), strerror(errno));
        duk_push_int(ctx, errno);
        duk_put_prop_string(ctx, -2, "errno");
        duk_throw(ctx);
        return -42;    /* control never returns here */
    } else {
        double t;
        t = tv.tv_sec + 1e-6*tv.tv_usec;
        duk_push_number(ctx, t);
        return 1;
    }
}


#define NANOS_PER_SEC 1000000000

/*
 * High precission time. If an optional array is given
 * it returns the difference.
 */
static duk_ret_t time_hrtime(duk_context* ctx) {
    uint64_t t = sjs_time_hrtime();
    uint64_t sec = t / NANOS_PER_SEC;
    uint64_t nsec = t % NANOS_PER_SEC;
    if (duk_is_array(ctx, 0)) {
        uint64_t sec0;
        uint64_t nsec0;
        duk_get_prop_index(ctx, 0, 0);
        sec0 = duk_require_uint(ctx, -1);
        duk_pop(ctx);
        duk_get_prop_index(ctx, 0, 1);
        nsec0 = duk_require_uint(ctx, -1);
        duk_pop(ctx);
        sec -= sec0;
        nsec -= nsec0;
        /* TODO: overflow? */
    }
    duk_push_array(ctx);
    duk_push_uint(ctx, sec);
    duk_put_prop_index(ctx, -2, 0);
    duk_push_uint(ctx, nsec);
    duk_put_prop_index(ctx, -2, 1);
    return 1;
}


static duk_ret_t time_sleep(duk_context* ctx) {
    struct timeval tv;
    double time;

    time = duk_require_number(ctx, 0);
    tv.tv_sec = (unsigned long) time;
    tv.tv_usec = (unsigned long)(time * 1000000) % 1000000;

    select(0, NULL, NULL, NULL, &tv);

    /* TODO: return remaining time in case of EINTR */
    duk_push_undefined(ctx);
    return 1;
}


static const duk_function_list_entry module_funcs[] = {
    /* name, function, nargs */
    { "time", time_time, 0 },
    { "hrtime", time_hrtime, 1 },
    { "sleep", time_sleep, 1 },
    { NULL, NULL, 0 }
};


DUK_EXTERNAL duk_ret_t sjs_mod_init(duk_context* ctx) {
    duk_push_object(ctx);
    duk_put_function_list(ctx, -1, module_funcs);
    return 1;
}

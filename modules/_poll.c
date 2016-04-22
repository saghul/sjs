
#include <assert.h>
#include <errno.h>
#include <poll.h>
#include <sjs/sjs.h>


/*
 * poll(2). Args:
 * - 0: fds array
 * - 1: timeout
 */
static duk_ret_t poll_poll(duk_context* ctx) {
    struct pollfd fds[1024];
    struct pollfd *pfds;
    double dtimeout;
    size_t i, n;
    int r, timeout;

    if (!duk_is_array(ctx, 0)) {
        duk_error(ctx, DUK_ERR_TYPE_ERROR, "'pfds' must be an array");
        return -42;    /* control never returns here */
    }
    duk_require_valid_index(ctx, 1);
    if (duk_is_null(ctx, 1)) {
        timeout = -1;
    } else {
        dtimeout = duk_require_number(ctx, 1);
        assert(!duk_is_nan(ctx, 1));
        timeout = dtimeout * 1000;
    }

    n = duk_get_length(ctx, 0);
    if (n > 1024) {
        pfds = malloc(sizeof(struct pollfd) * n);
        if (pfds == NULL) {
            SJS_THROW_ERRNO_ERROR2(ENOMEM);
            return -42;    /* control never returns here */
        }
    } else {
        pfds = fds;
    }

    for (i = 0; i < n; i++) {
        int fd, events;
        duk_get_prop_index(ctx, 0, i);
        duk_get_prop_string(ctx, -1, "fd");
        fd = duk_require_int(ctx, -1);
        duk_pop(ctx);
        duk_get_prop_string(ctx, -1, "events");
        events = duk_require_int(ctx, -1);
        duk_pop(ctx);
        pfds[i].fd = fd;
        pfds[i].events = events;
        pfds[i].revents = 0;
        duk_pop(ctx);
    }

    r = poll(pfds, n, timeout);
    if (r == -1) {
        if (pfds != fds) {
            free(pfds);
            pfds = NULL;
        }
        SJS_THROW_ERRNO_ERROR();
        return -42;    /* control never returns here */
    }

    /* TODO: consider duplicating the original array (or even modifying it) instead */
    duk_push_object(ctx);
    duk_push_int(ctx, r);
    duk_put_prop_string(ctx, -2, "nevents");
    duk_push_array(ctx);
    for (i = 0; i < n; i++) {
        duk_push_object(ctx);
        duk_push_int(ctx, pfds[i].fd);
        duk_put_prop_string(ctx, -2, "fd");
        duk_push_int(ctx, pfds[i].events);
        duk_put_prop_string(ctx, -2, "events");
        duk_push_int(ctx, pfds[i].revents);
        duk_put_prop_string(ctx, -2, "revents");
        duk_put_prop_index(ctx, -2, i);
    }
    duk_put_prop_string(ctx, -2, "fds");

    if (pfds != fds) {
        free(pfds);
        pfds = NULL;
    }

    return 1;
}


#define X(name) {#name, name}
static const duk_number_list_entry module_consts[] = {
    X(POLLIN),
    X(POLLOUT),
    X(POLLPRI),
#ifdef POLLRDHUP
    X(POLLRDHUP),
#endif
    X(POLLERR),
    X(POLLHUP),
    X(POLLNVAL),
    { NULL, 0.0 }
};
#undef X

static const duk_function_list_entry module_funcs[] = {
    /* name, function, nargs */
    { "poll", poll_poll, 2 },
    { NULL, NULL, 0 }
};


DUK_EXTERNAL duk_ret_t sjs_mod_init(duk_context* ctx) {
    duk_push_object(ctx);
    duk_put_function_list(ctx, -1, module_funcs);
    duk_push_object(ctx);
    duk_put_number_list(ctx, -1, module_consts);
    duk_put_prop_string(ctx, -2, "c");
    return 1;
}

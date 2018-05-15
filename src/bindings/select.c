
#include <assert.h>
#include <sys/select.h>

#include "../binding.h"
#include <sjs/sjs.h>


static void array2fdset(duk_context* ctx, int array_idx, fd_set* fdset, int* maxfd) {
    size_t i, n;

    n = duk_get_length(ctx, array_idx);
    for (i = 0; i < n; i++) {
        int fd;
        duk_get_prop_index(ctx, array_idx, i);
        fd = duk_require_int(ctx, -1);
        duk_pop(ctx);
        if (fd < 0 || fd >= FD_SETSIZE) {
            duk_error(ctx, DUK_ERR_RANGE_ERROR, "invalid fd: %d", fd);
            return;    /* control never returns here */
        }
        FD_SET(fd, fdset);
        if (fd > *maxfd) {
            *maxfd = fd;
        }
    }
}


static void fdset2array(duk_context* ctx, fd_set* fdset, int array_idx) {
    int fd, i;

    i = 0;
    for (fd = 0; fd < FD_SETSIZE; fd++) {
        if (FD_ISSET(fd, fdset)) {
            duk_push_int(ctx, fd);
            duk_put_prop_index(ctx, array_idx, i);
            i++;
        }
    }
}


/*
 * select(2). Args:
 * - 0: rfds
 * - 1: wfds
 * - 2: xfds
 * - 3: timeout
 */
static duk_ret_t select_select(duk_context* ctx) {
    fd_set rfds;
    fd_set wfds;
    fd_set xfds;
    int maxfd, r;
    struct timeval tv;
    struct timeval* tvp;
    duk_idx_t array_idx;

    if (!duk_is_array(ctx, 0) || !duk_is_array(ctx, 1) || !duk_is_array(ctx, 2)) {
        duk_error(ctx, DUK_ERR_TYPE_ERROR, "'rfds', 'wfds' and 'xfds' must be arrays");
        return -42;    /* control never returns here */
    }

    duk_require_valid_index(ctx, 3);
    if (duk_is_null(ctx, 3)) {
        tvp = NULL;
    } else {
        double timeout;
        timeout = duk_require_number(ctx, 3);
        assert(timeout >= 0);
        tv.tv_sec = (unsigned long) timeout;
        tv.tv_usec = (unsigned long)(timeout * 1000000) % 1000000;
        tvp = &tv;
    }

    /* convert 3 arrays to fd_set */
    FD_ZERO(&rfds);
    FD_ZERO(&wfds);
    FD_ZERO(&xfds);
    maxfd = -1;
    array2fdset(ctx, 0, &rfds, &maxfd);
    array2fdset(ctx, 1, &wfds, &maxfd);
    array2fdset(ctx, 2, &xfds, &maxfd);

    /* select! */
    r = select(maxfd+1, &rfds, &wfds, &xfds, tvp);
    if (r == -1) {
        SJS_THROW_ERRNO_ERROR();
        return -42;    /* control never returns here */
    }

    /* convert fd_set to array */
    duk_push_object(ctx);
    array_idx = duk_push_array(ctx);
    fdset2array(ctx, &rfds, array_idx);
    duk_put_prop_string(ctx, -2, "rfds");
    array_idx = duk_push_array(ctx);
    fdset2array(ctx, &wfds, array_idx);
    duk_put_prop_string(ctx, -2, "wfds");
    array_idx = duk_push_array(ctx);
    fdset2array(ctx, &xfds, array_idx);
    duk_put_prop_string(ctx, -2, "xfds");

    return 1;
}


#define X(name) {#name, name}
static const duk_number_list_entry module_consts[] = {
    X(FD_SETSIZE),
    { NULL, 0.0 }
};
#undef X


static const duk_function_list_entry module_funcs[] = {
    /* name, function, nargs */
    { "select", select_select, 4 },
    { NULL, NULL, 0 }
};


void sjs__binding_select_init(duk_context* ctx) {
    sjs__register_binding(ctx, "select", module_funcs, module_consts);
}


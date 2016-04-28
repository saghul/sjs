
#include <errno.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

#include <sjs/sjs.h>


/*
 * Read data from a fd. Args:
 * - 0: fd
 * - 1: nread
 * TODO:
 *  - use buffers
 */
static duk_ret_t io_read(duk_context* ctx) {
    int fd;
    ssize_t r;
    size_t nread;
    char* buf;

    fd = duk_require_int(ctx, 0);
    nread = duk_require_int(ctx, 1);
    buf = malloc(nread);
    if (!buf) {
        SJS_THROW_ERRNO_ERROR2(ENOMEM);
        return -42;    /* control never returns here */
    }

    r = read(fd, buf, nread);
    if (r < 0) {
        SJS_THROW_ERRNO_ERROR();
        return -42;    /* control never returns here */
    } else if (r == 0) {
        /* EOF */
        duk_push_string(ctx, "");
    } else {
        duk_push_lstring(ctx, buf, r);
    }

    return 1;
}


/*
 * Write data to a fd. Args:
 * - 0: fd
 * - 1: data
 * TODO:
 *  - use buffers
 */
static duk_ret_t io_write(duk_context* ctx) {
    int fd;
    ssize_t r;
    size_t len;
    const char* buf;

    fd = duk_require_int(ctx, 0);
    buf = duk_require_lstring(ctx, 1, &len);

    r = write(fd, buf, len);
    if (r < 0) {
        SJS_THROW_ERRNO_ERROR();
        return -42;    /* control never returns here */
    } else {
        duk_push_int(ctx, r);
    }

    return 1;
}


#define X(name) {#name, name}
static const duk_number_list_entry module_consts[] = {
    X(STDIN_FILENO),
    X(STDOUT_FILENO),
    X(STDERR_FILENO),
    { NULL, 0.0 }
};
#undef X


static const duk_function_list_entry module_funcs[] = {
    /* name, function, nargs */
    { "read", io_read, 2 },
    { "write", io_write, 2 },
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


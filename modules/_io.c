
#include <errno.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

#include <sjs/sjs.h>


/* TODO: DRY this? */
#define SJS__THROW_IO_ERROR(error)                                                              \
    do {                                                                                        \
        duk_push_error_object(ctx, DUK_ERR_ERROR, "[errno %d] %s", (error), strerror((error))); \
        duk_push_int(ctx, (error));                                                             \
        duk_put_prop_string(ctx, -2, "errno");                                                  \
        duk_throw(ctx);                                                                         \
    } while (0)                                                                                 \


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
        SJS__THROW_IO_ERROR(ENOMEM);
        return -42;    /* control never returns here */
    }

    r = read(fd, buf, nread);
    if (r < 0) {
        SJS__THROW_IO_ERROR(errno);
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
        SJS__THROW_IO_ERROR(errno);
        return -42;    /* control never returns here */
    } else {
        duk_push_int(ctx, r);
    }

    return 1;
}


static const duk_number_list_entry module_consts[] = {
    { "STDIN_FILENO", STDIN_FILENO },
    { "STDOUT_FILENO", STDOUT_FILENO },
    { "STDERR_FILENO", STDERR_FILENO },
    { NULL, 0.0 }
};


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


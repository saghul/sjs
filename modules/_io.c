
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <sjs/sjs.h>


/*
 * Open a file. Args:
 * - 0: path
 * - 1: mode
 */
static duk_ret_t io_fopen(duk_context* ctx) {
    const char* path;
    const char* mode;
    FILE* f;

    path = duk_require_string(ctx, 0);
    mode = duk_require_string(ctx, 1);

    f = fopen(path, mode);
    if (f == NULL) {
        SJS_THROW_ERRNO_ERROR();
        return -42;    /* control never returns here */
    } else {
        duk_push_pointer(ctx, (void*) f);
        return 1;
    }
}


/*
 * Read data from a file. Args:
 * - 0: FILE
 * - 1: nread (a number or a Buffer-ish object)
 */
static duk_ret_t io_fread(duk_context* ctx) {
    FILE* f;
    size_t nread, r;
    char* buf;
    char* alloc_buf = NULL;

    f = duk_require_pointer(ctx, 0);
    if (duk_is_number(ctx, 1)) {
        nread = duk_require_int(ctx, 1);
        alloc_buf = malloc(nread);
        if (!alloc_buf) {
            SJS_THROW_ERRNO_ERROR2(ENOMEM);
            return -42;    /* control never returns here */
        }
        buf = alloc_buf;
    } else {
        buf = duk_require_buffer_data(ctx, 1, &nread);
        if (buf == NULL || nread == 0) {
            duk_error(ctx, DUK_ERR_TYPE_ERROR, "invalid buffer");
            return -42;    /* control never returns here */
        }
    }

    r = fread(buf, 1, nread, f);
    if (ferror(f)) {
        free(alloc_buf);
        clearerr(f);
        SJS_THROW_ERRNO_ERROR2(EIO);
        return -42;    /* control never returns here */
    } else {
        if (alloc_buf) {
            /* return the string we read */
            duk_push_lstring(ctx, buf, r);
        } else {
            /* the data was written to the buffer, return how much we read */
            duk_push_int(ctx, r);
        }

        free(alloc_buf);
        return 1;
    }
}


/*
 * Write data to a file. Args:
 * - 0: FILE
 * - 1: data
 */
static duk_ret_t io_fwrite(duk_context* ctx) {
    FILE* f;
    size_t len, r;
    const char* buf;

    f = duk_require_pointer(ctx, 0);
    if (duk_is_string(ctx, 1)) {
        buf = duk_require_lstring(ctx, 1, &len);
    } else {
        buf = duk_require_buffer_data(ctx, 1, &len);
    }

    r = fwrite(buf, 1, len, f);
    if (ferror(f) || feof(f)) {
        clearerr(f);
        SJS_THROW_ERRNO_ERROR2(EIO);
        return -42;    /* control never returns here */
    } else {
        duk_push_int(ctx, r);
        return 1;
    }
}


/*
 * Close the file. Args:
 * - 0: FILE
 */
static duk_ret_t io_fclose(duk_context* ctx) {
    FILE* f;

    f = duk_require_pointer(ctx, 0);
    fclose(f);

    duk_push_undefined(ctx);
    return 1;
}


/*
 * Flush the file write buffer. Args:
 * - 0: FILE
 */
static duk_ret_t io_fflush(duk_context* ctx) {
    FILE* f;
    int r;

    f = duk_require_pointer(ctx, 0);

    r = fflush(f);
    if (r != 0) {
        SJS_THROW_ERRNO_ERROR();
        return -42;    /* control never returns here */
    }

    duk_push_undefined(ctx);
    return 1;
}


/*
 * Get the fd associated with a file. Args:
 * - 0: FILE
 */
static duk_ret_t io_fileno(duk_context* ctx) {
    FILE* f;
    int fd;

    f = duk_require_pointer(ctx, 0);
    fd = fileno(f);

    duk_push_int(ctx, fd);
    return 1;
}


static duk_ret_t io_stdin(duk_context* ctx) {
    duk_push_pointer(ctx, (void*) stdin);
    return 1;
}


static duk_ret_t io_stdout(duk_context* ctx) {
    duk_push_pointer(ctx, (void*) stdout);
    return 1;
}


static duk_ret_t io_stderr(duk_context* ctx) {
    duk_push_pointer(ctx, (void*) stderr);
    return 1;
}


#define X(name) {#name, name}
static const duk_number_list_entry module_consts[] = {
    { NULL, 0.0 }
};
#undef X


static const duk_function_list_entry module_funcs[] = {
    /* name, function, nargs */
    { "fopen", io_fopen, 2 },
    { "fread", io_fread, 2 },
    { "fwrite", io_fwrite, 2 },
    { "fclose", io_fclose, 1 },
    { "fflush", io_fflush, 1 },
    { "fileno", io_fileno, 1 },
    { "stdin", io_stdin, 0 },
    { "stdout", io_stdout, 0 },
    { "stderr", io_stderr, 0 },
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


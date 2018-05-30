
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "../binding.h"
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
 * Open a file given an fd. Args:
 * - 0: fd
 * - 1: mode
 */
static duk_ret_t io_fdopen(duk_context* ctx) {
    int fd;
    const char* mode;
    FILE* f;

    fd = duk_require_int(ctx, 0);
    mode = duk_require_string(ctx, 1);

    f = fdopen(fd, mode);
    if (f == NULL) {
        SJS_THROW_ERRNO_ERROR();
        return -42;    /* control never returns here */
    } else {
        duk_push_pointer(ctx, (void*) f);
        return 1;
    }
}


/*
 * Read a line from a file. Args:
 * - 0: FILE
 * - 1: nread (a number or a Buffer-ish object)
 */
static duk_ret_t io_fgets(duk_context* ctx) {
    FILE* f;
    size_t nread;
    char* r;
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

    r = fgets(buf, nread, f);
    if (r == NULL) {
        duk_push_string(ctx, "");
    } else {
        if (alloc_buf) {
            /* return the string we read */
            duk_push_string(ctx, r);
        } else {
            /* the data was written to the buffer, return how much we read */
            duk_push_int(ctx, strlen(r));
        }
    }
    free(alloc_buf);
    return 1;
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
    int create_buf = 0;

    f = duk_require_pointer(ctx, 0);
    if (duk_is_number(ctx, 1)) {
        nread = duk_require_int(ctx, 1);
        buf = duk_push_dynamic_buffer(ctx, nread);
        create_buf = 1;
    } else {
        buf = duk_require_buffer_data(ctx, 1, &nread);
        if (buf == NULL || nread == 0) {
            duk_error(ctx, DUK_ERR_TYPE_ERROR, "invalid buffer");
            return -42;    /* control never returns here */
        }
    }

    r = fread(buf, 1, nread, f);
    if (ferror(f)) {
        if (create_buf) {
            duk_pop(ctx);
        }
        clearerr(f);
        SJS_THROW_ERRNO_ERROR2(EIO);
        return -42;    /* control never returns here */
    }

    if (create_buf) {
        /* return the string we read */
        duk_resize_buffer(ctx, -1, r);
    } else {
        /* the data was written to the buffer, return how much we read */
        duk_push_int(ctx, r);
    }

    return 1;
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


/*
 * Set buffering type for file object. Args:
 * - 0: FILE
 * - 1: mode (0 is unbuffered, 1 is line buffered)
 */
static duk_ret_t io_setvbuf(duk_context* ctx) {
    FILE* f;
    int mode;
    size_t size;

    f = duk_require_pointer(ctx, 0);
    mode = duk_require_int(ctx, 1);

    if (mode == 0) {
        mode = _IONBF;
        size = 0;
    } else if (mode == 1) {
        mode = _IOLBF;
        size = BUFSIZ;
    } else {
        abort();
    }

    /*
     * Ignore erros. According to the man page: "It may set errno on failure.". Sigh.
     */
    (void) setvbuf(f, NULL, mode, size);

    duk_push_undefined(ctx);
    return 1;
}


static const duk_function_list_entry module_funcs[] = {
    /* name, function, nargs */
    { "fopen", io_fopen, 2 },
    { "fdopen", io_fdopen, 2 },
    { "fgets", io_fgets, 2 },
    { "fread", io_fread, 2 },
    { "fwrite", io_fwrite, 2 },
    { "fclose", io_fclose, 1 },
    { "fflush", io_fflush, 1 },
    { "fileno", io_fileno, 1 },
    { "setvbuf", io_setvbuf, 2 },
    { NULL, NULL, 0 }
};


void sjs__binding_io_init(duk_context* ctx) {
    sjs__register_binding(ctx, "io", module_funcs, NULL);
}


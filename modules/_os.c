
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <stdlib.h>
#include <unistd.h>

#include <sjs/sjs.h>


/*
 * Open a file for low level I/O. Args:
 * - 0: path
 * - 1: flags
 * - 2: mode
 */
static duk_ret_t os_open(duk_context* ctx) {
    const char* path;
    int flags, mode, r;

    path = duk_require_string(ctx, 0);
    flags = duk_require_int(ctx, 1);
    mode = duk_require_int(ctx, 2);

    r = open(path, flags, mode);
    if (r == -1) {
        SJS_THROW_ERRNO_ERROR();
        return -42;    /* control never returns here */
    } else {
        duk_push_int(ctx, r);
        return 1;
    }
}


/*
 * Read data from a fd. Args:
 * - 0: fd
 * - 1: nread (a number or a Buffer-ish object)
 */
static duk_ret_t os_read(duk_context* ctx) {
    int fd;
    ssize_t r;
    size_t nread;
    char* buf;
    char* alloc_buf = NULL;

    fd = duk_require_int(ctx, 0);
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

    r = read(fd, buf, nread);
    if (r < 0) {
        free(alloc_buf);
        SJS_THROW_ERRNO_ERROR();
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
 * Write data to a fd. Args:
 * - 0: fd
 * - 1: data
 */
static duk_ret_t os_write(duk_context* ctx) {
    int fd;
    ssize_t r;
    size_t len;
    const char* buf;

    fd = duk_require_int(ctx, 0);
    if (duk_is_string(ctx, 1)) {
        buf = duk_require_lstring(ctx, 1, &len);
    } else {
        buf = duk_require_buffer_data(ctx, 1, &len);
    }

    r = write(fd, buf, len);
    if (r < 0) {
        SJS_THROW_ERRNO_ERROR();
        return -42;    /* control never returns here */
    } else {
        duk_push_int(ctx, r);
    }

    return 1;
}


/*
 * Close the fd. Args:
 * - 0: fd
 */
static duk_ret_t os_close(duk_context* ctx) {
    int fd;

    fd = duk_require_int(ctx, 0);
    close(fd);

    duk_push_undefined(ctx);
    return 1;
}


static duk_ret_t os_abort(duk_context* ctx) {
    (void) ctx;
    abort();
    return -42;    /* control never returns here */
}


static duk_ret_t os_pipe(duk_context* ctx) {
    int fds[2];
    int r;

    r = pipe(fds);
    if (r < 0) {
        SJS_THROW_ERRNO_ERROR();
        return -42;    /* control never returns here */
    } else {
        duk_push_array(ctx);
        duk_push_int(ctx, fds[0]);
    	duk_put_prop_index(ctx, -2, 0);
        duk_push_int(ctx, fds[1]);
    	duk_put_prop_index(ctx, -2, 1);
        return 1;
    }
}


static duk_ret_t os_isatty(duk_context* ctx) {
    int fd, r;

    fd = duk_require_int(ctx, 0);
    r = isatty(fd);

    duk_push_boolean(ctx, r);
    return 1;
}


static duk_ret_t os_ttyname(duk_context* ctx) {
    int fd, r;
    char path[256];

    fd = duk_require_int(ctx, 0);
    r = ttyname_r(fd, path, sizeof(path));
    if (r < 0) {
        SJS_THROW_ERRNO_ERROR();
        return -42;    /* control never returns here */
    } else {
        duk_push_string(ctx, path);
        return 1;
    }
}


static duk_ret_t os_getcwd(duk_context* ctx) {
    char* r;
    char path[8192];

    r = getcwd(path, sizeof(path));
    if (r == NULL) {
        SJS_THROW_ERRNO_ERROR();
        return -42;    /* control never returns here */
    } else {
        duk_push_string(ctx, path);
        return 1;
    }
}


static int sjs__scandir_filter(const struct dirent* dent) {
    return strcmp(dent->d_name, ".") != 0 && strcmp(dent->d_name, "..") != 0;
}

static duk_ret_t os_scandir(duk_context* ctx) {
    int r;
    struct dirent** dents;
    struct dirent* dent;
    const char* path;

    path = duk_require_string(ctx, 0);

    r = scandir(path, &dents, sjs__scandir_filter, alphasort);
    if (r < 0) {
        SJS_THROW_ERRNO_ERROR();
        return -42;    /* control never returns here */
    } else {
        duk_push_array(ctx);
        for (int i = 0; i < r; i++) {
            dent = dents[i];
            duk_push_string(ctx, dent->d_name);
            duk_put_prop_index(ctx, -2, i);
            free(dent);
        }
        free(dents);
        return 1;
    }
}


#if defined(__APPLE__)
static inline double sjs__timespec2ms(const struct timespec ts) {
    return (double)(ts.tv_sec * 1000) + (double)(ts.tv_nsec / 1000000);
}
#endif

/*
 * stat(2). Args:
 * - 0: path
 * - 1: function to build the stat result
 */
static duk_ret_t os_stat(duk_context* ctx) {
    int r;
    const char* path;
    struct stat st;

    path = duk_require_string(ctx, 0);
    duk_require_function(ctx, 1);

    r = stat(path, &st);
    if (r < 0) {
        SJS_THROW_ERRNO_ERROR();
        return -42;    /* control never returns here */
    } else {
        /* prepare the call to the given function */
        duk_dup(ctx, 1);
        duk_push_object(ctx);
        /* [... func obj ] */

        duk_push_uint(ctx, st.st_dev);
        duk_push_uint(ctx, st.st_mode);
        duk_push_uint(ctx, st.st_nlink);
        duk_push_uint(ctx, st.st_uid);
        duk_push_uint(ctx, st.st_gid);
        duk_push_uint(ctx, st.st_rdev);
        duk_push_uint(ctx, st.st_ino);
        duk_push_uint(ctx, st.st_size);
        duk_push_uint(ctx, st.st_blksize);
        duk_push_uint(ctx, st.st_blocks);
#if defined(__APPLE__)
        duk_push_uint(ctx, st.st_flags);
        duk_push_uint(ctx, st.st_gen);
#else
        duk_push_uint(ctx, 0);    /* st_flags */
        duk_push_uint(ctx, 0);    /* st_gen */
#endif

	/* times, convert to ms */
#if defined(__APPLE__)
        duk_push_number(ctx, sjs__timespec2ms(st.st_atimespec));
        duk_push_number(ctx, sjs__timespec2ms(st.st_mtimespec));
        duk_push_number(ctx, sjs__timespec2ms(st.st_ctimespec));
        duk_push_number(ctx, sjs__timespec2ms(st.st_birthtimespec));
#else
	duk_push_number(ctx, st.st_atime * 1000);
	duk_push_number(ctx, st.st_mtime * 1000);
	duk_push_number(ctx, st.st_ctime * 1000);
	duk_push_number(ctx, st.st_ctime * 1000);    /* st_birthtim */
#endif

        duk_call_method(ctx, 16 /* number of args */);
        return 1;
    }
}


static duk_ret_t os_unlink(duk_context* ctx) {
    const char* path;
    int r;

    path = duk_require_string(ctx, 0);

    r = unlink(path);
    if (r < 0) {
        SJS_THROW_ERRNO_ERROR();
        return -42;    /* control never returns here */
    } else {
        duk_push_undefined(ctx);
        return 1;
    }
}


/* read `size` bytes from /dev/urandom */
static int sjs__urandom(void* vbuf, size_t size) {
    int fd;
    ssize_t r;
    char* buf = vbuf;

    fd = open("/dev/urandom", O_RDONLY);
    if (fd < 0) {
        return -errno;
    }

    while (size > 0) {
        do {
            r = read(fd, buf, size);
        } while (r < 0 && errno == EINTR);
        if (r < 0) {
            r = -errno;
            close(fd);
            return r;
        }
        buf += r;
        size -= r;
    }

    close(fd);
    return 0;
}

#ifdef SYS_getrandom
/* Linux getrandom(2) */
static int sjs__getrandom(void* vbuf, size_t size) {
    int r;
    char* buf = vbuf;

    while (size > 0) {
        do {
            r = syscall(SYS_getrandom, vbuf, size, 0);
        } while (r < 0 && errno == EINTR);
        if (r < 0) {
            return -errno;
        }
        buf += r;
        size -= r;
    }
    return 0;
}
#endif

/* read `size` bytes from the system random source (CSPRNG) */
static int sjs__sys_random(void* vbuf, size_t size) {
#ifdef SYS_getrandom
    static int no_getrandom;
    if (!no_getrandom) {
        int r = sjs__getrandom(vbuf, size);
        if (r == -ENOSYS) {
            no_getrandom = 1;
        } else {
            return r;
        }
    }
#endif
    return sjs__urandom(vbuf, size);
}

static duk_ret_t os_urandom(duk_context* ctx) {
    char* buf;
    size_t nbytes;
    int r;

    buf = duk_require_buffer_data(ctx, 0, &nbytes);

    /* TODO: use getrandom(2) on Linux */
    r = sjs__sys_random(buf, nbytes);
    if (r < 0) {
        SJS_THROW_ERRNO_ERROR2(-r);
        return -42;    /* control never returns here */
    } else {
        duk_push_undefined(ctx);
        return 1;
    }
}


#define X(name) {#name, name}
static const duk_number_list_entry module_consts[] = {
    /* flags for open */
    X(O_APPEND),
    X(O_CREAT),
    X(O_EXCL),
    X(O_RDONLY),
    X(O_RDWR),
    X(O_SYNC),
    X(O_TRUNC),
    X(O_WRONLY),
    /* stat types */
    X(S_IFDIR),
    X(S_IFCHR),
    X(S_IFBLK),
    X(S_IFREG),
    X(S_IFIFO),
    X(S_IFLNK),
    X(S_IFSOCK),
    /* names of permissions bits */
    X(S_ISUID),
    X(S_ISGID),
    X(S_ISVTX),
    X(S_IREAD),
    X(S_IWRITE),
    X(S_IEXEC),
    X(S_IRWXU),
    X(S_IRUSR),
    X(S_IWUSR),
    X(S_IXUSR),
    X(S_IRWXG),
    X(S_IRGRP),
    X(S_IWGRP),
    X(S_IXGRP),
    X(S_IRWXO),
    X(S_IROTH),
    X(S_IWOTH),
    X(S_IXOTH),
    { NULL, 0.0 }
};
#undef X


static const duk_function_list_entry module_funcs[] = {
    /* name, function, nargs */
    { "open", os_open, 3 },
    { "read", os_read, 2 },
    { "write", os_write, 2 },
    { "close", os_close, 1 },
    { "abort", os_abort, 0 },
    { "pipe", os_pipe, 0 },
    { "isatty", os_isatty, 1 },
    { "ttyname", os_ttyname, 1 },
    { "getcwd", os_getcwd, 0 },
    { "scandir", os_scandir, 1 },
    { "stat", os_stat, 2 },
    { "unlink", os_unlink, 1 },
    { "urandom", os_urandom, 1 },
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


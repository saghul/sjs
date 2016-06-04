
#include <assert.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>

#if defined(__APPLE__)
# include <crt_externs.h>
# define environ (*_NSGetEnviron())
#endif

#include "../src/platform-inl.h"
#include <sjs/sjs.h>


static int sjs__open(const char *pathname, int flags, mode_t mode) {
    static int no_cloexec_support;
    int r;

    if (no_cloexec_support == 0) {
#if defined(O_CLOEXEC)
        r = open(pathname, flags | O_CLOEXEC, mode);
        if (r >= 0) {
            return r;
        }
        if (errno != EINVAL) {
            return -errno;
        }
        no_cloexec_support = 1;
#endif
    }

    int fd;
    r = open(pathname, flags, mode);
    if (r < 0) {
        return -errno;
    }
    fd = r;
    r = sjs__cloexec(fd, 1);
    if (r < 0) {
        sjs__close(fd);
        return r;
    }
    return fd;
}

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

    r = sjs__open(path, flags, mode);
    if (r < 0) {
        SJS_THROW_ERRNO_ERROR2(-r);
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
    int fd, r;

    fd = duk_require_int(ctx, 0);
    r = sjs__close(fd);
    if (r < 0) {
        SJS_THROW_ERRNO_ERROR2(-r);
        return -42;    /* control never returns here */
    }

    duk_push_undefined(ctx);
    return 1;
}


static duk_ret_t os_abort(duk_context* ctx) {
    (void) ctx;
    abort();
    return -42;    /* control never returns here */
}


static int sjs__pipe(int fds[2]) {
    int r;
#if defined(__linux__)
    r = pipe2(fds, O_CLOEXEC);
    if (r < 0) {
        return -errno;
    }
    return r;
#endif
    r = pipe(fds);
    if (r < 0) {
        return -errno;
    }
    r = sjs__cloexec(fds[0], 1);
    if (r < 0) {
        goto error;
    }
    r = sjs__cloexec(fds[1], 1);
    if (r < 0) {
        goto error;
    }
    return r;
error:
    sjs__close(fds[0]);
    sjs__close(fds[1]);
    fds[0] = -1;
    fds[1] = -1;
    return r;
}

static duk_ret_t os_pipe(duk_context* ctx) {
    int fds[2];
    int r;

    r = sjs__pipe(fds);
    if (r < 0) {
        SJS_THROW_ERRNO_ERROR2(-r);
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
    int fd, err;
    ssize_t r;
    struct stat st;
    char* buf = vbuf;

    err = open("/dev/urandom", O_RDONLY);
    if (err < 0) {
        return -errno;
    }
    fd = err;
    if (fstat(fd, &st) < 0) {
        sjs__close(fd);
        return -errno;
    }
    if (!S_ISCHR(st.st_mode)) {
        /* not a character device! */
        sjs__close(fd);
        return -EIO;
    }

    while (size > 0) {
        do {
            r = read(fd, buf, size);
        } while (r < 0 && errno == EINTR);
        if (r < 0) {
            sjs__close(fd);
            return -errno;
        }
        buf += r;
        size -= r;
    }

    sjs__close(fd);
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
#ifdef __APPLE__
    arc4random_buf(vbuf, size);
    return 0;
#endif
    return sjs__urandom(vbuf, size);
}

static duk_ret_t os_urandom(duk_context* ctx) {
    char* buf;
    size_t nbytes;
    int r;

    buf = duk_require_buffer_data(ctx, 0, &nbytes);

    r = sjs__sys_random(buf, nbytes);
    if (r < 0) {
        SJS_THROW_ERRNO_ERROR2(-r);
        return -42;    /* control never returns here */
    } else {
        duk_push_undefined(ctx);
        return 1;
    }
}


static duk_ret_t os_fork(duk_context* ctx) {
    pid_t pid;

    pid = fork();
    if (pid < 0) {
        SJS_THROW_ERRNO_ERROR();
        return -42;    /* control never returns here */
    }

    duk_push_uint(ctx, pid);
    return 1;
}


static duk_ret_t os_execv(duk_context* ctx) {
    const char* path;
    size_t nargs;
    int r;

    path = duk_require_string(ctx, 0);
    assert(duk_is_array(ctx, 1));
    nargs = duk_get_length(ctx, 1);

    char* args[nargs+1];
    for (size_t i = 0; i < nargs; i++) {
        duk_get_prop_index(ctx, 1, i);
        args[i] = (char*) duk_require_string(ctx, -1);
        duk_pop(ctx);
    }
    args[nargs] = NULL;

    r = execv(path, args);
    if (r < 0) {
        SJS_THROW_ERRNO_ERROR();
        return -42;    /* control never returns here */
    }

    return -42;    /* control never returns here */
}


static duk_ret_t os_execve(duk_context* ctx) {
    const char* path;
    size_t nargs, nenv;
    int r;

    path = duk_require_string(ctx, 0);
    assert(duk_is_array(ctx, 1));
    assert(duk_is_array(ctx, 2));
    nargs = duk_get_length(ctx, 1);
    nenv = duk_get_length(ctx, 2);

    char* args[nargs+1];
    for (size_t i = 0; i < nargs; i++) {
        duk_get_prop_index(ctx, 1, i);
        args[i] = (char*) duk_require_string(ctx, -1);
        duk_pop(ctx);
    }
    args[nargs] = NULL;

    char* env[nenv+1];
    for (size_t i = 0; i < nenv; i++) {
        duk_get_prop_index(ctx, 2, i);
        env[i] = (char*) duk_require_string(ctx, -1);
        duk_pop(ctx);
    }
    env[nenv] = NULL;

    r = execve(path, args, env);
    if (r < 0) {
        SJS_THROW_ERRNO_ERROR();
        return -42;    /* control never returns here */
    }

    return -42;    /* control never returns here */
}


static duk_ret_t os_execvp(duk_context* ctx) {
    const char* file;
    size_t nargs;
    int r;

    file = duk_require_string(ctx, 0);
    assert(duk_is_array(ctx, 1));
    nargs = duk_get_length(ctx, 1);

    char* args[nargs+1];
    for (size_t i = 0; i < nargs; i++) {
        duk_get_prop_index(ctx, 1, i);
        args[i] = (char*) duk_require_string(ctx, -1);
        duk_pop(ctx);
    }
    args[nargs] = NULL;

    r = execvp(file, args);
    if (r < 0) {
        SJS_THROW_ERRNO_ERROR();
        return -42;    /* control never returns here */
    }

    return -42;    /* control never returns here */
}


static duk_ret_t os_execvpe(duk_context* ctx) {
    const char* file;
    size_t nargs, nenv;
    int r;

    file = duk_require_string(ctx, 0);
    assert(duk_is_array(ctx, 1));
    assert(duk_is_array(ctx, 2));
    nargs = duk_get_length(ctx, 1);
    nenv = duk_get_length(ctx, 2);

    char* args[nargs+1];
    for (size_t i = 0; i < nargs; i++) {
        duk_get_prop_index(ctx, 1, i);
        args[i] = (char*) duk_require_string(ctx, -1);
        duk_pop(ctx);
    }
    args[nargs] = NULL;

    char* env[nenv+1];
    for (size_t i = 0; i < nenv; i++) {
        duk_get_prop_index(ctx, 2, i);
        env[i] = (char*) duk_require_string(ctx, -1);
        duk_pop(ctx);
    }
    env[nenv] = NULL;

#if defined(__APPLE__)
    environ = env;
    r = execvp(file, args);
#else
    r = execvpe(file, args, env);
#endif
    if (r < 0) {
        SJS_THROW_ERRNO_ERROR();
        return -42;    /* control never returns here */
    }

    return -42;    /* control never returns here */
}


static duk_ret_t os_waitpid(duk_context* ctx) {
    pid_t pid, r;
    int options, status;

    pid = duk_require_int(ctx, 0);
    options = duk_require_int(ctx, 1);
    status = 0;

    r = waitpid(pid, &status, options);
    if (r < 0) {
        SJS_THROW_ERRNO_ERROR();
        return -42;    /* control never returns here */
    } else {
        duk_push_object(ctx);
        duk_push_uint(ctx, r);
        duk_put_prop_string(ctx, -2, "pid");
        duk_push_int(ctx, status);
        duk_put_prop_string(ctx, -2, "status");
        return 1;
    }
}


static duk_ret_t os_WIFEXITED(duk_context* ctx) {
    int status;
    status = duk_require_int(ctx, 0);
    duk_push_boolean(ctx, WIFEXITED(status));
    return 1;
}


static duk_ret_t os_WEXITSTATUS(duk_context* ctx) {
    int status;
    status = duk_require_int(ctx, 0);
    duk_push_int(ctx, WEXITSTATUS(status));
    return 1;
}


static duk_ret_t os_WIFSIGNALED(duk_context* ctx) {
    int status;
    status = duk_require_int(ctx, 0);
    duk_push_boolean(ctx, WIFSIGNALED(status));
    return 1;
}


static duk_ret_t os_WTERMSIG(duk_context* ctx) {
    int status;
    status = duk_require_int(ctx, 0);
    duk_push_int(ctx, WTERMSIG(status));
    return 1;
}


static duk_ret_t os_WIFSTOPPED(duk_context* ctx) {
    int status;
    status = duk_require_int(ctx, 0);
    duk_push_boolean(ctx, WIFSTOPPED(status));
    return 1;
}


static duk_ret_t os_WSTOPSIG(duk_context* ctx) {
    int status;
    status = duk_require_int(ctx, 0);
    duk_push_int(ctx, WSTOPSIG(status));
    return 1;
}


static duk_ret_t os_WIFCONTINUED(duk_context* ctx) {
    int status;
    status = duk_require_int(ctx, 0);
    duk_push_boolean(ctx, WIFCONTINUED(status));
    return 1;
}


static duk_ret_t os_cloexec(duk_context* ctx) {
    int fd, r;
    duk_bool_t set;

    fd = duk_require_int(ctx, 0);
    set = duk_require_boolean(ctx, 1);

    r = sjs__cloexec(fd, set);
    if (r < 0) {
        SJS_THROW_ERRNO_ERROR2(-r);
        return -42;    /* control never returns here */
    } else {
        duk_push_undefined(ctx);
        return 1;
    }
}


static duk_ret_t os_nonblock(duk_context* ctx) {
    int fd, r;
    duk_bool_t set;

    fd = duk_require_int(ctx, 0);
    set = duk_require_boolean(ctx, 1);

    r = sjs__nonblock(fd, set);
    if (r < 0) {
        SJS_THROW_ERRNO_ERROR2(-r);
        return -42;    /* control never returns here */
    } else {
        duk_push_undefined(ctx);
        return 1;
    }
}


static duk_ret_t os_getpid(duk_context* ctx) {
    pid_t pid = getpid();
    duk_push_uint(ctx, pid);
    return 1;
}


static duk_ret_t os_getppid(duk_context* ctx) {
    pid_t pid = getppid();
    duk_push_uint(ctx, pid);
    return 1;
}


static duk_ret_t os_dup(duk_context* ctx) {
    int oldfd, newfd, r;

    oldfd = duk_require_int(ctx, 0);
    r = dup(oldfd);
    if (r < 0) {
        SJS_THROW_ERRNO_ERROR();
        return -42;    /* control never returns here */
    }
    newfd = r;
    r = sjs__cloexec(newfd, 1);
    if (r < 0) {
        sjs__close(newfd);
        SJS_THROW_ERRNO_ERROR2(-r);
        return -42;    /* control never returns here */
    }
    duk_push_int(ctx, newfd);
    return 1;
}


static duk_ret_t os_dup2(duk_context* ctx) {
    int oldfd, newfd, r;
    duk_bool_t cloexec;

    oldfd = duk_require_int(ctx, 0);
    newfd = duk_require_int(ctx, 1);
    cloexec = duk_require_boolean(ctx, 2);

#if defined(__linux__)
    r = dup3(oldfd, newfd, cloexec ? O_CLOEXEC : 0);
    if (r < 0) {
        SJS_THROW_ERRNO_ERROR();
        return -42;    /* control never returns here */
    }
    goto end;
#endif
    r = dup2(oldfd, newfd);
    if (r < 0) {
        SJS_THROW_ERRNO_ERROR();
        return -42;    /* control never returns here */
    }
    r = sjs__cloexec(newfd, cloexec);
    if (r < 0) {
        sjs__close(newfd);
        SJS_THROW_ERRNO_ERROR2(-r);
        return -42;    /* control never returns here */
    }
    goto end;    /* pacify compiler */

end:
    duk_push_int(ctx, newfd);
    return 1;
}


static duk_ret_t os_chdir(duk_context* ctx) {
    const char* path;
    int r;

    path = duk_require_string(ctx, 0);
    r = chdir(path);
    if (r < 0) {
        SJS_THROW_ERRNO_ERROR();
        return -42;    /* control never returns here */
    }

    duk_push_undefined(ctx);
    return 1;
}


static duk_ret_t os_exit(duk_context* ctx) {
    int status = duk_require_int(ctx, 0);
    exit(status);
    return -42;    /* control never returns here */
}


static duk_ret_t os__exit(duk_context* ctx) {
    int status = duk_require_int(ctx, 0);
    _exit(status);
    return -42;    /* control never returns here */
}


static duk_ret_t os_setsid(duk_context* ctx) {
    pid_t r;
    r = setsid();
    if (r < 0) {
        SJS_THROW_ERRNO_ERROR();
        return -42;    /* control never returns here */
    } else {
        duk_push_uint(ctx, r);
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
    /* options for waitpid */
    X(WCONTINUED),
    X(WNOHANG),
    X(WUNTRACED),
    { NULL, 0.0 }
};
#undef X


static const duk_function_list_entry module_funcs[] = {
    /* name, function, nargs */
    { "open",                   os_open,            3 },
    { "read",                   os_read,            2 },
    { "write",                  os_write,           2 },
    { "close",                  os_close,           1 },
    { "abort",                  os_abort,           0 },
    { "pipe",                   os_pipe,            0 },
    { "isatty",                 os_isatty,          1 },
    { "ttyname",                os_ttyname,         1 },
    { "getcwd",                 os_getcwd,          0 },
    { "scandir",                os_scandir,         1 },
    { "stat",                   os_stat,            2 },
    { "unlink",                 os_unlink,          1 },
    { "urandom",                os_urandom,         1 },
    { "fork",                   os_fork,            0 },
    { "execv",                  os_execv,           2 },
    { "execve",                 os_execve,          3 },
    { "execvp",                 os_execvp,          2 },
    { "execvpe",                os_execvpe,         3 },
    { "waitpid",                os_waitpid,         2 },
    { "WIFEXITED",              os_WIFEXITED,       1 },
    { "WEXITSTATUS",            os_WEXITSTATUS,     1 },
    { "WIFSIGNALED",            os_WIFSIGNALED,     1 },
    { "WTERMSIG",               os_WTERMSIG,        1 },
    { "WIFSTOPPED",             os_WIFSTOPPED,      1 },
    { "WSTOPSIG",               os_WSTOPSIG,        1 },
    { "WIFCONTINUED",           os_WIFCONTINUED,    1 },
    { "cloexec",                os_cloexec,         2 },
    { "nonblock",               os_nonblock,        2 },
    { "getpid",                 os_getpid,          0 },
    { "getppid",                os_getppid,         0 },
    { "dup",                    os_dup,             1 },
    { "dup2",                   os_dup2,            3 },
    { "chdir",                  os_chdir,           1 },
    { "exit",                   os_exit,            1 },
    { "_exit",                  os__exit,           1 },
    { "setsid",                 os_setsid,          0 },
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


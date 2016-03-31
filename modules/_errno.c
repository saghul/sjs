
#include <errno.h>
#include <sjs/sjs.h>


static duk_ret_t errno_strerror(duk_context* ctx) {
    int r;
    int err;
    char buf[512];

    err = duk_require_int(ctx, 0);
    r = strerror_r(err, buf, sizeof(buf));
    if (r == 0) {
        duk_push_string(ctx, buf);
    } else {
        duk_push_undefined(ctx);
    }

    return 1;
}


static const duk_number_list_entry module_consts[] = {
#ifdef EPERM
    { "EPERM", EPERM },
#endif
#ifdef ENOENT
    { "ENOENT", ENOENT },
#endif
#ifdef ESRCH
    { "ESRCH", ESRCH },
#endif
#ifdef EINTR
    { "EINTR", EINTR },
#endif
#ifdef EIO
    { "EIO", EIO },
#endif
#ifdef ENXIO
    { "ENXIO", ENXIO },
#endif
#ifdef E2BIG
    { "E2BIG", E2BIG },
#endif
#ifdef ENOEXEC
    { "ENOEXEC", ENOEXEC },
#endif
#ifdef EBADF
    { "EBADF", EBADF },
#endif
#ifdef ECHILD
    { "ECHILD", ECHILD },
#endif
#ifdef EDEADLK
    { "EDEADLK", EDEADLK },
#endif
#ifdef ENOMEM
    { "ENOMEM", ENOMEM },
#endif
#ifdef EACCES
    { "EACCES", EACCES },
#endif
#ifdef EFAULT
    { "EFAULT", EFAULT },
#endif
#ifdef ENOTBLK
    { "ENOTBLK", ENOTBLK },
#endif
#ifdef EBUSY
    { "EBUSY", EBUSY },
#endif
#ifdef EEXIST
    { "EEXIST", EEXIST },
#endif
#ifdef EXDEV
    { "EXDEV", EXDEV },
#endif
#ifdef ENODEV
    { "ENODEV", ENODEV },
#endif
#ifdef ENOTDIR
    { "ENOTDIR", ENOTDIR },
#endif
#ifdef EISDIR
    { "EISDIR", EISDIR },
#endif
#ifdef EINVAL
    { "EINVAL", EINVAL },
#endif
#ifdef ENFILE
    { "ENFILE", ENFILE },
#endif
#ifdef EMFILE
    { "EMFILE", EMFILE },
#endif
#ifdef ENOTTY
    { "ENOTTY", ENOTTY },
#endif
#ifdef ETXTBSY
    { "ETXTBSY", ETXTBSY },
#endif
#ifdef EFBIG
    { "EFBIG", EFBIG },
#endif
#ifdef ENOSPC
    { "ENOSPC", ENOSPC },
#endif
#ifdef ESPIPE
    { "ESPIPE", ESPIPE },
#endif
#ifdef EROFS
    { "EROFS", EROFS },
#endif
#ifdef EMLINK
    { "EMLINK", EMLINK },
#endif
#ifdef EPIPE
    { "EPIPE", EPIPE },
#endif
#ifdef EDOM
    { "EDOM", EDOM },
#endif
#ifdef ERANGE
    { "ERANGE", ERANGE },
#endif
#ifdef EAGAIN
    { "EAGAIN", EAGAIN },
#endif
#ifdef EWOULDBLOCK
    { "EWOULDBLOCK", EWOULDBLOCK },
#endif
#ifdef EINPROGRESS
    { "EINPROGRESS", EINPROGRESS },
#endif
#ifdef EALREADY
    { "EALREADY", EALREADY },
#endif
#ifdef ENOTSOCK
    { "ENOTSOCK", ENOTSOCK },
#endif
#ifdef EDESTADDRREQ
    { "EDESTADDRREQ", EDESTADDRREQ },
#endif
#ifdef EMSGSIZE
    { "EMSGSIZE", EMSGSIZE },
#endif
#ifdef EPROTOTYPE
    { "EPROTOTYPE", EPROTOTYPE },
#endif
#ifdef ENOPROTOOPT
    { "ENOPROTOOPT", ENOPROTOOPT },
#endif
#ifdef EPROTONOSUPPORT
    { "EPROTONOSUPPORT", EPROTONOSUPPORT },
#endif
#ifdef ESOCKTNOSUPPORT
    { "ESOCKTNOSUPPORT", ESOCKTNOSUPPORT },
#endif
#ifdef ENOTSUP
    { "ENOTSUP", ENOTSUP },
#endif
#ifdef EPFNOSUPPORT
    { "EPFNOSUPPORT", EPFNOSUPPORT },
#endif
#ifdef EAFNOSUPPORT
    { "EAFNOSUPPORT", EAFNOSUPPORT },
#endif
#ifdef EADDRINUSE
    { "EADDRINUSE", EADDRINUSE },
#endif
#ifdef EADDRNOTAVAIL
    { "EADDRNOTAVAIL", EADDRNOTAVAIL },
#endif
#ifdef ENETDOWN
    { "ENETDOWN", ENETDOWN },
#endif
#ifdef ENETUNREACH
    { "ENETUNREACH", ENETUNREACH },
#endif
#ifdef ENETRESET
    { "ENETRESET", ENETRESET },
#endif
#ifdef ECONNABORTED
    { "ECONNABORTED", ECONNABORTED },
#endif
#ifdef ECONNRESET
    { "ECONNRESET", ECONNRESET },
#endif
#ifdef ENOBUFS
    { "ENOBUFS", ENOBUFS },
#endif
#ifdef EISCONN
    { "EISCONN", EISCONN },
#endif
#ifdef ENOTCONN
    { "ENOTCONN", ENOTCONN },
#endif
#ifdef ESHUTDOWN
    { "ESHUTDOWN", ESHUTDOWN },
#endif
#ifdef ETOOMANYREFS
    { "ETOOMANYREFS", ETOOMANYREFS },
#endif
#ifdef ETIMEDOUT
    { "ETIMEDOUT", ETIMEDOUT },
#endif
#ifdef ECONNREFUSED
    { "ECONNREFUSED", ECONNREFUSED },
#endif
#ifdef ELOOP
    { "ELOOP", ELOOP },
#endif
#ifdef ENAMETOOLONG
    { "ENAMETOOLONG", ENAMETOOLONG },
#endif
#ifdef EHOSTDOWN
    { "EHOSTDOWN", EHOSTDOWN },
#endif
#ifdef EHOSTUNREACH
    { "EHOSTUNREACH", EHOSTUNREACH },
#endif
#ifdef ENOTEMPTY
    { "ENOTEMPTY", ENOTEMPTY },
#endif
#ifdef EPROCLIM
    { "EPROCLIM", EPROCLIM },
#endif
#ifdef EUSERS
    { "EUSERS", EUSERS },
#endif
#ifdef EDQUOT
    { "EDQUOT", EDQUOT },
#endif
#ifdef ESTALE
    { "ESTALE", ESTALE },
#endif
#ifdef EREMOTE
    { "EREMOTE", EREMOTE },
#endif
#ifdef EBADRPC
    { "EBADRPC", EBADRPC },
#endif
#ifdef ERPCMISMATCH
    { "ERPCMISMATCH", ERPCMISMATCH },
#endif
#ifdef EPROGUNAVAIL
    { "EPROGUNAVAIL", EPROGUNAVAIL },
#endif
#ifdef EPROGMISMATCH
    { "EPROGMISMATCH", EPROGMISMATCH },
#endif
#ifdef EPROCUNAVAIL
    { "EPROCUNAVAIL", EPROCUNAVAIL },
#endif
#ifdef ENOLCK
    { "ENOLCK", ENOLCK },
#endif
#ifdef ENOSYS
    { "ENOSYS", ENOSYS },
#endif
#ifdef EFTYPE
    { "EFTYPE", EFTYPE },
#endif
#ifdef EAUTH
    { "EAUTH", EAUTH },
#endif
#ifdef ENEEDAUTH
    { "ENEEDAUTH", ENEEDAUTH },
#endif
#ifdef EPWROFF
    { "EPWROFF", EPWROFF },
#endif
#ifdef EDEVERR
    { "EDEVERR", EDEVERR },
#endif
#ifdef EOVERFLOW
    { "EOVERFLOW", EOVERFLOW },
#endif
#ifdef EBADEXEC
    { "EBADEXEC", EBADEXEC },
#endif
#ifdef EBADARCH
    { "EBADARCH", EBADARCH },
#endif
#ifdef ESHLIBVERS
    { "ESHLIBVERS", ESHLIBVERS },
#endif
#ifdef EBADMACHO
    { "EBADMACHO", EBADMACHO },
#endif
#ifdef ECANCELED
    { "ECANCELED", ECANCELED },
#endif
#ifdef EIDRM
    { "EIDRM", EIDRM },
#endif
#ifdef ENOMSG
    { "ENOMSG", ENOMSG },
#endif
#ifdef EILSEQ
    { "EILSEQ", EILSEQ },
#endif
#ifdef ENOATTR
    { "ENOATTR", ENOATTR },
#endif
#ifdef EBADMSG
    { "EBADMSG", EBADMSG },
#endif
#ifdef EMULTIHOP
    { "EMULTIHOP", EMULTIHOP },
#endif
#ifdef ENODATA
    { "ENODATA", ENODATA },
#endif
#ifdef ENOLINK
    { "ENOLINK", ENOLINK },
#endif
#ifdef ENOSR
    { "ENOSR", ENOSR },
#endif
#ifdef ENOSTR
    { "ENOSTR", ENOSTR },
#endif
#ifdef EPROTO
    { "EPROTO", EPROTO },
#endif
#ifdef ETIME
    { "ETIME", ETIME },
#endif
#ifdef EOPNOTSUPP
    { "EOPNOTSUPP", EOPNOTSUPP },
#endif
#ifdef ENOPOLICY
    { "ENOPOLICY", ENOPOLICY },
#endif
#ifdef ENOTRECOVERABLE
    { "ENOTRECOVERABLE", ENOTRECOVERABLE },
#endif
#ifdef EOWNERDEAD
    { "EOWNERDEAD", EOWNERDEAD },
#endif
    { NULL, 0.0 }
};


static const duk_function_list_entry module_funcs[] = {
    /* name, function, nargs */
    { "strerror", errno_strerror, 1 },
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

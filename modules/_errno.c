
#ifdef _GNU_SOURCE
# undef _GNU_SOURCE
#endif
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


#define X(name) {#name, name}
static const duk_number_list_entry module_consts[] = {
#ifdef EPERM
    X(EPERM),
#endif
#ifdef ENOENT
    X(ENOENT),
#endif
#ifdef ESRCH
    X(ESRCH),
#endif
#ifdef EINTR
    X(EINTR),
#endif
#ifdef EIO
    X(EIO),
#endif
#ifdef ENXIO
    X(ENXIO),
#endif
#ifdef E2BIG
    X(E2BIG),
#endif
#ifdef ENOEXEC
    X(ENOEXEC),
#endif
#ifdef EBADF
    X(EBADF),
#endif
#ifdef ECHILD
    X(ECHILD),
#endif
#ifdef EDEADLK
    X(EDEADLK),
#endif
#ifdef ENOMEM
    X(ENOMEM),
#endif
#ifdef EACCES
    X(EACCES),
#endif
#ifdef EFAULT
    X(EFAULT),
#endif
#ifdef ENOTBLK
    X(ENOTBLK),
#endif
#ifdef EBUSY
    X(EBUSY),
#endif
#ifdef EEXIST
    X(EEXIST),
#endif
#ifdef EXDEV
    X(EXDEV),
#endif
#ifdef ENODEV
    X(ENODEV),
#endif
#ifdef ENOTDIR
    X(ENOTDIR),
#endif
#ifdef EISDIR
    X(EISDIR),
#endif
#ifdef EINVAL
    X(EINVAL),
#endif
#ifdef ENFILE
    X(ENFILE),
#endif
#ifdef EMFILE
    X(EMFILE),
#endif
#ifdef ENOTTY
    X(ENOTTY),
#endif
#ifdef ETXTBSY
    X(ETXTBSY),
#endif
#ifdef EFBIG
    X(EFBIG),
#endif
#ifdef ENOSPC
    X(ENOSPC),
#endif
#ifdef ESPIPE
    X(ESPIPE),
#endif
#ifdef EROFS
    X(EROFS),
#endif
#ifdef EMLINK
    X(EMLINK),
#endif
#ifdef EPIPE
    X(EPIPE),
#endif
#ifdef EDOM
    X(EDOM),
#endif
#ifdef ERANGE
    X(ERANGE),
#endif
#ifdef EAGAIN
    X(EAGAIN),
#endif
#ifdef EWOULDBLOCK
    X(EWOULDBLOCK),
#endif
#ifdef EINPROGRESS
    X(EINPROGRESS),
#endif
#ifdef EALREADY
    X(EALREADY),
#endif
#ifdef ENOTSOCK
    X(ENOTSOCK),
#endif
#ifdef EDESTADDRREQ
    X(EDESTADDRREQ),
#endif
#ifdef EMSGSIZE
    X(EMSGSIZE),
#endif
#ifdef EPROTOTYPE
    X(EPROTOTYPE),
#endif
#ifdef ENOPROTOOPT
    X(ENOPROTOOPT),
#endif
#ifdef EPROTONOSUPPORT
    X(EPROTONOSUPPORT),
#endif
#ifdef ESOCKTNOSUPPORT
    X(ESOCKTNOSUPPORT),
#endif
#ifdef ENOTSUP
    X(ENOTSUP),
#endif
#ifdef EPFNOSUPPORT
    X(EPFNOSUPPORT),
#endif
#ifdef EAFNOSUPPORT
    X(EAFNOSUPPORT),
#endif
#ifdef EADDRINUSE
    X(EADDRINUSE),
#endif
#ifdef EADDRNOTAVAIL
    X(EADDRNOTAVAIL),
#endif
#ifdef ENETDOWN
    X(ENETDOWN),
#endif
#ifdef ENETUNREACH
    X(ENETUNREACH),
#endif
#ifdef ENETRESET
    X(ENETRESET),
#endif
#ifdef ECONNABORTED
    X(ECONNABORTED),
#endif
#ifdef ECONNRESET
    X(ECONNRESET),
#endif
#ifdef ENOBUFS
    X(ENOBUFS),
#endif
#ifdef EISCONN
    X(EISCONN),
#endif
#ifdef ENOTCONN
    X(ENOTCONN),
#endif
#ifdef ESHUTDOWN
    X(ESHUTDOWN),
#endif
#ifdef ETOOMANYREFS
    X(ETOOMANYREFS),
#endif
#ifdef ETIMEDOUT
    X(ETIMEDOUT),
#endif
#ifdef ECONNREFUSED
    X(ECONNREFUSED),
#endif
#ifdef ELOOP
    X(ELOOP),
#endif
#ifdef ENAMETOOLONG
    X(ENAMETOOLONG),
#endif
#ifdef EHOSTDOWN
    X(EHOSTDOWN),
#endif
#ifdef EHOSTUNREACH
    X(EHOSTUNREACH),
#endif
#ifdef ENOTEMPTY
    X(ENOTEMPTY),
#endif
#ifdef EPROCLIM
    X(EPROCLIM),
#endif
#ifdef EUSERS
    X(EUSERS),
#endif
#ifdef EDQUOT
    X(EDQUOT),
#endif
#ifdef ESTALE
    X(ESTALE),
    { "ESTALE", ESTALE },
#endif
#ifdef EREMOTE
    X(EREMOTE),
#endif
#ifdef EBADRPC
    X(EBADRPC),
#endif
#ifdef ERPCMISMATCH
    X(ERPCMISMATCH),
#endif
#ifdef EPROGUNAVAIL
    X(EPROGUNAVAIL),
#endif
#ifdef EPROGMISMATCH
    X(EPROGMISMATCH),
#endif
#ifdef EPROCUNAVAIL
    X(EPROCUNAVAIL),
#endif
#ifdef ENOLCK
    X(ENOLCK),
#endif
#ifdef ENOSYS
    X(ENOSYS),
#endif
#ifdef EFTYPE
    X(EFTYPE),
#endif
#ifdef EAUTH
    X(EAUTH),
#endif
#ifdef ENEEDAUTH
    X(ENEEDAUTH),
#endif
#ifdef EPWROFF
    X(EPWROFF),
#endif
#ifdef EDEVERR
    X(EDEVERR),
#endif
#ifdef EOVERFLOW
    X(EOVERFLOW),
#endif
#ifdef EBADEXEC
    X(EBADEXEC),
#endif
#ifdef EBADARCH
    X(EBADARCH),
#endif
#ifdef ESHLIBVERS
    X(ESHLIBVERS),
#endif
#ifdef EBADMACHO
    X(EBADMACHO),
#endif
#ifdef ECANCELED
    X(ECANCELED),
#endif
#ifdef EIDRM
    X(EIDRM),
#endif
#ifdef ENOMSG
    X(ENOMSG),
#endif
#ifdef EILSEQ
    X(EILSEQ),
#endif
#ifdef ENOATTR
    X(ENOATTR),
#endif
#ifdef EBADMSG
    X(EBADMSG),
#endif
#ifdef EMULTIHOP
    X(EMULTIHOP),
#endif
#ifdef ENODATA
    X(ENODATA),
#endif
#ifdef ENOLINK
    X(ENOLINK),
#endif
#ifdef ENOSR
    X(ENOSR),
#endif
#ifdef ENOSTR
    X(ENOSTR),
#endif
#ifdef EPROTO
    X(EPROTO),
#endif
#ifdef ETIME
    X(ETIME),
#endif
#ifdef EOPNOTSUPP
    X(EOPNOTSUPP),
#endif
#ifdef ENOPOLICY
    X(ENOPOLICY),
#endif
#ifdef ENOTRECOVERABLE
    X(ENOTRECOVERABLE),
#endif
#ifdef EOWNERDEAD
    X(EOWNERDEAD),
#endif
    { NULL, 0.0 }
};
#undef X


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

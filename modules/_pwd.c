
#include <sys/types.h>
#include <pwd.h>
#include <unistd.h>

#include <sjs/sjs.h>


/*
 * Convert a struct passwd into an object
 *    struct passwd {
 *       char   *pw_name;
 *       char   *pw_passwd;
 *       uid_t   pw_uid;
 *       gid_t   pw_gid;
 *       char   *pw_gecos;
 *       char   *pw_dir;
 *       char   *pw_shell;
 *   };
 */
static void passwd2obj(duk_context* ctx, struct passwd* pw) {
    duk_idx_t idx;
    idx = duk_push_object(ctx);

    duk_push_string(ctx, pw->pw_name);
    duk_put_prop_string(ctx, idx, "name");

    duk_push_string(ctx, pw->pw_passwd);
    duk_put_prop_string(ctx, idx, "passwd");

    duk_push_int(ctx, pw->pw_uid);
    duk_put_prop_string(ctx, idx, "uid");

    duk_push_int(ctx, pw->pw_gid);
    duk_put_prop_string(ctx, idx, "gid");

    duk_push_string(ctx, pw->pw_gecos);
    duk_put_prop_string(ctx, idx, "gecos");

    duk_push_string(ctx, pw->pw_dir);
    duk_put_prop_string(ctx, idx, "dir");

    duk_push_string(ctx, pw->pw_shell);
    duk_put_prop_string(ctx, idx, "shell");
}


static duk_ret_t pwd_getpwnam(duk_context* ctx) {
    const char* name;
    long initsize;
    char* buf;
    size_t bufsize;
    struct passwd pw;
    struct passwd* rpw;
    int r;

    name = duk_require_string(ctx, 0);

    initsize = sysconf(_SC_GETPW_R_SIZE_MAX);
    if (initsize <= 0) {
        bufsize = 4096;
    } else {
        bufsize = (size_t) initsize;
    }

    buf = NULL;

    for (;;) {
        free(buf);
        buf = malloc(bufsize);

        if (buf == NULL) {
            SJS_THROW_ERRNO_ERROR2(ENOMEM);
            return -42;    /* control never returns here */
        }

        r = getpwnam_r(name, &pw, buf, bufsize, &rpw);

        if (r != ERANGE) {
            break;
        }

        bufsize *= 2;
    }

    if (r != 0) {
        SJS_THROW_ERRNO_ERROR2(r);
        return -42;    /* control never returns here */
    }

    if (rpw == NULL) {
        free(buf);
        duk_push_null(ctx);
        return 1;
    } else {
        passwd2obj(ctx, &pw);
        return 1;
    }
}


static duk_ret_t pwd_getpwuid(duk_context* ctx) {
    uid_t uid;
    long initsize;
    char* buf;
    size_t bufsize;
    struct passwd pw;
    struct passwd* rpw;
    int r;

    uid = duk_require_int(ctx, 0);

    initsize = sysconf(_SC_GETPW_R_SIZE_MAX);
    if (initsize <= 0) {
        bufsize = 4096;
    } else {
        bufsize = (size_t) initsize;
    }

    buf = NULL;

    for (;;) {
        free(buf);
        buf = malloc(bufsize);

        if (buf == NULL) {
            SJS_THROW_ERRNO_ERROR2(ENOMEM);
            return -42;    /* control never returns here */
        }

        r = getpwuid_r(uid, &pw, buf, bufsize, &rpw);

        if (r != ERANGE) {
            break;
        }

        bufsize *= 2;
    }

    if (r != 0) {
        SJS_THROW_ERRNO_ERROR2(r);
        return -42;    /* control never returns here */
    }

    if (rpw == NULL) {
        free(buf);
        duk_push_null(ctx);
        return 1;
    } else {
        passwd2obj(ctx, &pw);
        return 1;
    }
}


static const duk_function_list_entry module_funcs[] = {
    { "getpwnam", pwd_getpwnam, 1 /*nargs*/ },
    { "getpwuid", pwd_getpwuid, 1 /*nargs*/ },
    { NULL, NULL, 0 }
};


DUK_EXTERNAL duk_ret_t sjs_mod_init(duk_context* ctx) {
    duk_push_object(ctx);
    duk_put_function_list(ctx, -1, module_funcs);
    return 1;
}

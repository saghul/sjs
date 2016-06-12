
#include <assert.h>
#include <dlfcn.h>
#include <errno.h>
#include <libgen.h>
#include <pwd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "internal.h"
#include "duktape.h"
#include "duk_module_node.h"


typedef duk_ret_t (*sjs_mod_init_f)(duk_context*);

//#define SJS_DEBUG_MODULE_LOADER 1


/*
 * Load a jsdll addon located at 'path'. If successful it puts the exported object
 * on top of the value stack.
 *
 * TODO: keep track of the loaded modules to dlclose() them on shutdown.
 */
static int load_jsdll(duk_context* ctx, const char* path) {
    void* lib = dlopen(path, RTLD_LAZY | RTLD_LOCAL);
    if (lib) {
        sjs_mod_init_f mod_init;
        *(void**)(&mod_init) = dlsym(lib, "sjs_mod_init");
        if (mod_init) {
            duk_push_c_function(ctx, mod_init, 1 /*nargs*/);
            duk_call(ctx, 0);
            return 0;
        } else {
            dlclose(lib);
            return -1;
        }
    } else {
        return -1;
    }
}


static inline int sjs__path_normalize_impl(const char* path,
                                           char* normalized_path,
                                           size_t normalized_path_len,
                                           int use_realpath) {
    assert(path && normalized_path && normalized_path_len);

    const char* ptr;

    /* expand ~ and ~user */
    if (path[0] == '~') {
        /* relative to home path: ~/foo/bar or ~user/foo/bar */
        char tmp[8192];
        char* home;
        char* sep = strchr(path, '/');
        int pos;
        if (sep) {
            pos = sep - path;
        } else {
            pos = strlen(path);
        }
        if (pos == 1) {
            home = getenv("HOME");
            if (!home) {
                char buf[4096];
                struct passwd pw;
                struct passwd *pwresult;
                uid_t uid = getuid();
                int r = getpwuid_r(uid, &pw, buf, sizeof(buf), &pwresult);
                if (r != 0) {
                    return -r;
                }
                if (pwresult == NULL) {
                    return -ENOENT;
                }
                home = pw.pw_dir;
            }
        } else {
            char name[256];
            char buf[4096];
            struct passwd pw;
            struct passwd *pwresult;
            snprintf(name, pos, "%s", path+1);
            int r = getpwnam_r(name, &pw, buf, sizeof(buf), &pwresult);
            if (r != 0) {
                return -r;
            }
            if (pwresult == NULL) {
                return -ENOENT;
            }
            home = pw.pw_dir;
        }
        snprintf(tmp, sizeof(tmp), "%s%s", home, path+pos);
        ptr = tmp;
    } else {
        ptr = path;
    }

    if (use_realpath) {
        if (realpath(ptr, normalized_path) == NULL) {
            return -errno;
        }
    } else {
        sjs__strlcpy(normalized_path, ptr, normalized_path_len);
    }

    return 0;
}


int sjs__path_normalize(const char* path, char* normalized_path, size_t normalized_path_len) {
    return sjs__path_normalize_impl(path, normalized_path, normalized_path_len, 1);
}


int sjs__path_expanduser(const char* path, char* normalized_path, size_t normalized_path_len) {
    return sjs__path_normalize_impl(path, normalized_path, normalized_path_len, 0);
}


static int str_endswith(const char* str, const char* suffix) {
    size_t str_len = strlen(str);
    size_t suffix_len = strlen(suffix);
    return (str_len > suffix_len && strcmp(str + str_len - suffix_len, suffix) == 0);
}


/* Resolve a module's ID
 *
 * Entry stack: [ requested_id parent_id ]
 */
static inline duk_ret_t resolve_module_helper(duk_context* ctx,
                                       const char* base_path,
                                       const char* requested_id) {
    static const char* filenames[] = {
        "%s/%s",
        "%s/%s.js",
        "%s/%s.jsdll",
        "%s/%s/index.js",
    };

    char resolved_id[8192];
    char tmp[8192];
    struct stat st;

    for (size_t i = 0; i < ARRAY_SIZE(filenames); ++i) {
        snprintf(tmp, sizeof(tmp), filenames[i], base_path, requested_id);
#ifdef SJS_DEBUG_MODULE_LOADER
        printf("Trying to resolve ID: %s\n", tmp);
#endif
        if (sjs__path_normalize(tmp, resolved_id, sizeof(resolved_id)) == 0 &&
                stat(resolved_id, &st) == 0 && S_ISREG(st.st_mode)) {
#ifdef SJS_DEBUG_MODULE_LOADER
            printf("Resolved ID: %s\n", resolved_id);
#endif
            duk_push_string(ctx, resolved_id);
            return 0;
        }
    }

    return -1;
}


static duk_ret_t cb_resolve_module(duk_context *ctx) {
    const char *requested_id = duk_get_string(ctx, 0);
    const char *parent_id = duk_get_string(ctx, 1);  /* calling module */

#ifdef SJS_DEBUG_MODULE_LOADER
    printf("Requested ID: %s Parent ID: %s\n", requested_id, parent_id);
#endif

    if (strncmp(requested_id, "./", 2) == 0 || strncmp(requested_id, "../", 3) == 0) {
        /* path is relative to parent path */
        char tmp[8192];
        if (strlen(parent_id) == 0) {
            sjs__strlcpy(tmp, "./", sizeof(tmp));
        } else if (str_endswith(parent_id, ".js")) {
            sjs__strlcpy(tmp, dirname((char*) parent_id), sizeof(tmp));
        } else {
            sjs__strlcpy(tmp, parent_id, sizeof(tmp));
        }
        if (resolve_module_helper(ctx, tmp, requested_id) == 0) {
            /* the resolved module ID is at the top of the stack */
            return 1;
        }
    } else {
        /* path needs to be looked up in system.path */
        const char* path;
        size_t n;
        int found;
        duk_get_global_string(ctx, "_system");
        duk_get_prop_string(ctx, -1, "path");
        duk_remove(ctx, -2);
        n = duk_get_length(ctx, -1);
        found = 0;
        for (size_t i = 0; i < n; ++i) {
            duk_get_prop_index(ctx, -1, i);
            path = duk_require_string(ctx, -1);
#ifdef SJS_DEBUG_MODULE_LOADER
            printf("System path: %s\n", path);
#endif
            duk_pop(ctx);
            if (resolve_module_helper(ctx, path, requested_id) == 0) {
                found = 1;
                break;
            }
        }
        if (found) {
            /* the resolved module ID is at the top of the stack */
            duk_remove(ctx, -2);
            return 1;
        }
        duk_pop(ctx);
    }

    duk_error(ctx, DUK_ERR_ERROR, "Module not found");
    /* duk_error doesn't return */
    return -42;
}


/* Load a module given it's ID
 *
 * Entry stack: [ resolved_id exports module ]
 */
static duk_ret_t cb_load_module(duk_context *ctx) {
    const char* resolved_id = duk_require_string(ctx, 0);

#ifdef SJS_DEBUG_MODULE_LOADER
    printf("Loading module with ID: %s\n", resolved_id);
#endif

    if (str_endswith(resolved_id, ".js")) {
        char* data;
        int len;
        len = sjs__file_read(resolved_id, &data);
        if (len < 0) {
            goto error;
        }
        if (strncmp(data, "#!", 2) == 0) {
            memcpy((void*) data, "//", 2);
        }
        duk_push_lstring(ctx, data, len);
        free(data);
        return 1;
    } else if (str_endswith(resolved_id, ".jsdll")) {
        if (load_jsdll(ctx, resolved_id) < 0) {
            goto error;
        }
        /* the top of the stack has the exports object */
        duk_put_prop_string(ctx, 2 /*idx of 'module'*/, "exports");
        return 0;
    }

error:
    duk_error(ctx, DUK_ERR_ERROR, "Module could not be loaded");
    /* duk_error doesn't return */
    return -42;
}


void sjs__setup_commonjs(duk_context* ctx) {
    duk_push_object(ctx);
    duk_push_c_function(ctx, cb_resolve_module, DUK_VARARGS);
    duk_put_prop_string(ctx, -2, "resolve");
    duk_push_c_function(ctx, cb_load_module, DUK_VARARGS);
    duk_put_prop_string(ctx, -2, "load");
    duk_module_node_init(ctx);
}


#include <assert.h>
#include <dlfcn.h>
#include <errno.h>
#include <pwd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "internal.h"
#include "duktape.h"


typedef duk_ret_t (*sjs_mod_init_f)(duk_context*);


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


static void path_join(char* path, size_t size, const char* p) {
    char sep;
    size_t pathlen;

    assert(path && p);

    pathlen = strlen(path);
    sep = '\0';
    if (path[pathlen-1] != '/') {
        if (p[0] != '/') {
            sep = '/';
        }
    } else {
        if (p[0] == '/') {
            p++;
        }
    }

    snprintf(path+pathlen, size-pathlen, "%c%s", sep, p);
}


int sjs__path_normalize(const char* path, char* normalized_path, size_t normalized_path_len) {
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
        snprintf(tmp, sizeof(tmp), "%s/%s", home, path+pos);
        ptr = tmp;
    } else {
        ptr = path;
    }

    if (realpath(ptr, normalized_path) == NULL) {
        return -errno;
    }

    return 0;
}


static int str_endswith(const char* str, const char* suffix) {
    size_t str_len = strlen(str);
    size_t suffix_len = strlen(suffix);
    return (str_len > suffix_len && strcmp(str + str_len - suffix_len, suffix) == 0);
}


duk_ret_t sjs__modsearch(duk_context* ctx) {
    /* nargs is 4:
     * 0: id
     * 1: require
     * 2: exports
     * 3: module
     */

    /*
     * - search the module in the following paths
     *   .
     *   ./modules
     *  /usr/lib/sjs/modules
     *  /usr/local/lib/lodules
     *  ~/.local/sjs/modules
     *
     *  - if there is an extension, try to load the file straight
     *  - else: append js and try
     *  - else: stat and if it's a dir try to add /index.js
     */

    ssize_t len;
    char* data;
    const char* id;
    char tmp[8192];
    int found_js;
    int found_jsdll;

    found_js = 0;
    found_jsdll = 0;
    data = NULL;
    len = -1;
    id = duk_get_string(ctx, 0);

    /* iterate over system.path */
    duk_get_global_string(ctx, "system");
    duk_get_prop_string(ctx, -1, "path");
    duk_remove(ctx, -2);
    duk_enum(ctx, -1, DUK_ENUM_ARRAY_INDICES_ONLY | DUK_ENUM_SORT_ARRAY_INDICES);
    /* [ ... path enum ] */

    while (duk_next(ctx, -1, 1)) {
        struct stat st;
        const char* path = duk_get_string(ctx, -1);
        duk_pop_2(ctx);    /* pop key and value off the stack */

        if (sjs__path_normalize(path, tmp, sizeof(tmp)) < 0) {
            continue;
        }

        path_join(tmp, sizeof(tmp), id);

        if (stat(tmp, &st) == 0) {
            /* path exists, check if file or directory */
            if (S_ISREG(st.st_mode)) {
                /* file, check if .js or .jsdll */
                if (str_endswith(tmp, ".js")) {
                    len = sjs__file_read(tmp, &data);
                    if (len < 0) {
                        continue;
                    }
                    found_js = 1;
                    break;
                } else if (str_endswith(tmp, ".jsdll")) {
                    if (load_jsdll(ctx, tmp) == 0) {
                        found_jsdll = 1;
                        break;
                    }
                } else {
                    continue;
                }
            } else if (S_ISDIR(st.st_mode)) {
                /* directory, check for index.js or index.jsdll */
                path_join(tmp, sizeof(tmp), "index.js");
                len = sjs__file_read(tmp, &data);
                if (len < 0) {
                    /* no index.js, try index.jsdll */
                    sjs__strlcat(tmp, "dll", sizeof(tmp));
                    if (load_jsdll(ctx, tmp) == 0) {
                        found_jsdll = 1;
                        break;
                    }
                    continue;
                }
                /* fix require.id */
                {
                    char tmpid[1024];
                    snprintf(tmpid, sizeof(tmpid), "%s/index", id);
                    duk_push_string(ctx, "id");
                    duk_push_string(ctx, tmpid);
                    duk_def_prop(ctx, 1 /* index of 'require' */, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_FORCE);
                }
                found_js = 1;
                break;
            }
        } else {
            /* path doesn't exist, try to add .js or .jsdll */
            sjs__strlcat(tmp, ".js", sizeof(tmp));
            len = sjs__file_read(tmp, &data);
            if (len < 0) {
                /* no index.js, try index.jsdll */
                sjs__strlcat(tmp, "dll", sizeof(tmp));
                if (load_jsdll(ctx, tmp) == 0) {
                    found_jsdll = 1;
                    break;
                }
                continue;
            }
            found_js = 1;
            break;
        }
    }

    if (found_js) {
        /* remove shebang if present */
        if (strncmp(data, "#!", 2) == 0) {
            memcpy((void*) data, "//", 2);
        }
        duk_pop_2(ctx);    /* pop the enum and path off the stack */
        duk_push_sprintf(ctx, "var __file__ = \"%s\";", tmp);
        duk_push_lstring(ctx, data, len);
        duk_concat(ctx, 2);
        free(data);
        return 1;
    } else if (found_jsdll) {
        /* the top of the stack has the exports object */
        duk_put_prop_string(ctx, 3 /*idx of 'module'*/, "exports");
        duk_pop_2(ctx);    /* pop the enum and path off the stack */
        return 0;
    } else {
        duk_pop_2(ctx);    /* pop the enum and path off the stack */
        duk_error(ctx, DUK_ERR_ERROR, "Module '%s' not found", id);
        /* duk_error doesn't return */
        return -42;
    }
}

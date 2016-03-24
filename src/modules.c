
#include <assert.h>
#include <dlfcn.h>
#include <errno.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "internal.h"
#include "duktape.h"


typedef duk_ret_t (*sjs_mod_init_f)(duk_context*);


static ssize_t read_file(const char* path, char** data) {
    FILE* f;
    long fsize;

    *data = NULL;

    f = fopen(path, "rb");
    if (!f) {
        return -errno;
    }

    if (fseek(f, 0, SEEK_END) < 0) {
        return -errno;
    }

    fsize = ftell(f);
    if (fsize < 0) {
        return -errno;
    }
    rewind(f);

    *data = malloc(fsize);
    if (!*data) {
        return -ENOMEM;
    }

    fread(*data, fsize, 1, f);
    if (ferror(f)) {
        fclose(f);
        free(data);
        return -EIO;
    }

    fclose(f);
    return fsize;
}


/*
 * Load a jsdll addon located at 'path'. If successful it puts the exported object
 * on top of the value stack.
 *
 * TODO: keep track of the loaded modules to dlclose() them on shutdown.
 */
static int load_jsdll(duk_context* ctx, const char* path) {
    void* lib = dlopen(path, RTLD_NOW|RTLD_LOCAL);
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


static void path_join(char* path, const char* p) {
    assert(path && p);

    if (path[strlen(path)-1] != '/') {
        if (p[0] != '/') {
            strcat(path, "/");
        }
    } else {
        if (p[0] == '/') {
            p++;
        }
    }

    strcat(path, p);
}


static int normalize_path(const char* path, const char*filename, char* normalized_path) {
    assert(path && filename && normalized_path);

    if (path[0] == '/') {
        /* absolute path: /usr/lib/foo */
        strcpy(normalized_path, path);
    } else if (path[0] == '~') {
        /* relative to home path: ~/foo/bar or ~user/foo/bar */
        char* home;
        char* pathptr;
        if (strlen(path) == 1 || path[1] == '/') {
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
            pathptr = (char*) path + 1;
        } else {
            char name[256];
            char buf[4096];
            struct passwd pw;
            struct passwd *pwresult;
            size_t namelen;
            char* p = strchr(path, '/');
            if (!p) {
                namelen = strlen(path) - 1;    /* remove leading ~ */
            } else {
                namelen = p - path;
            }
            snprintf(name, namelen, "%s", path+1);
            int r = getpwnam_r(name, &pw, buf, sizeof(buf), &pwresult);
            if (r != 0) {
                return -r;
            }
            if (pwresult == NULL) {
                return -ENOENT;
            }
            home = pw.pw_dir;
            pathptr = (char*) path + namelen;
        }
        strcpy(normalized_path, home);
        path_join(normalized_path, pathptr);
    } else {
        if (realpath(path, normalized_path) == NULL) {
            return -errno;
        }
    }

    path_join(normalized_path, filename);
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
    const char* filename;
    char tmp[8192];
    int found_js;
    int found_jsdll;

    found_js = 0;
    found_jsdll = 0;
    data = NULL;
    len = -1;
    filename = duk_get_string(ctx, 0);

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

        if (normalize_path(path, filename, tmp) < 0) {
            continue;
        }

        if (stat(tmp, &st) == 0) {
            /* path exists, check if file or directory */
            if (S_ISREG(st.st_mode)) {
                /* file, check if .js or .jsdll */
                if (str_endswith(tmp, ".js")) {
                    len = read_file(tmp, &data);
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
                path_join(tmp, "index.js");
                len = read_file(tmp, &data);
                if (len < 0) {
                    /* no index.js, try index.jsdll */
                    strcat(tmp, "dll");
                    if (load_jsdll(ctx, tmp) == 0) {
                        found_jsdll = 1;
                        break;
                    }
                    continue;
                }
                found_js = 1;
                break;
            }
        } else {
            /* path doesn't exist, try to add .js or .jsdll */
            strcat(tmp, ".js");
            len = read_file(tmp, &data);
            if (len < 0) {
                /* no index.js, try index.jsdll */
                strcat(tmp, "dll");
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
        duk_pop_2(ctx);    /* pop the enum and path off the stack */
        duk_push_lstring(ctx, data, len);
        free(data);
        return 1;
    } else if (found_jsdll) {
        /* the top of the stack has the exports object */
        duk_put_prop_string(ctx, 3 /*idx of 'module'*/, "exports");
        duk_pop_2(ctx);    /* pop the enum and path off the stack */
        return 0;
    } else {
        duk_pop_2(ctx);    /* pop the enum and path off the stack */
        duk_error(ctx, DUK_ERR_ERROR, "Module '%s' not found", filename);
        /* duk_error doesn't return */
        return -42;
    }
}

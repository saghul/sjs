
#include <assert.h>
#include <errno.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "internal.h"
#include "duktape.h"


static const char* default_search_paths[] = {
    ".",
    "./modules",
    "/usr/lib/sjs/modules",
    "/usr/local/lib/sjs/modules",
    "~saghul/.local/sjs/modules",
    NULL
};


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
    int found;

    found = 0;
    data = NULL;
    len = -1;
    filename = duk_get_string(ctx, 0);

    for (int i = 0; default_search_paths[i]; i++) {
        const char* path = default_search_paths[i];

        if (normalize_path(path, filename, tmp) < 0) {
            continue;
        }

        if (str_endswith(tmp, ".js")) {
            len = read_file(tmp, &data);
            if (len < 0) {
                continue;
            }
            found = 1;
            break;
        } else {
            struct stat st;
            if (stat(tmp, &st) != 0) {
                continue;
            }
            if (S_ISDIR(st.st_mode)) {
                /* it's a directory, try to add /index.js */
                path_join(tmp, "index.js");
                len = read_file(tmp, &data);
                if (len < 0) {
                    continue;
                }
                found = 1;
                break;
            } else {
                continue;
            }
        }
    }

    if (found) {
        duk_push_lstring(ctx, data, len);
        free(data);
        return 1;
    } else {
        duk_error(ctx, DUK_ERR_ERROR, "Module '%s' not found", filename);
        /* duk_error doesn't return */
        return -42;
    }
}

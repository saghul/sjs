
#ifdef __APPLE__
# include <crt_externs.h>
# define environ (*_NSGetEnviron())
#else
extern char** environ;
#endif

#include "duktape.h"
#include "build.h"
#include "internal.h"
#include "version.h"


static const char* default_search_paths[] = {
    ".",
    "./modules",
    "/usr/lib/sjs/modules",
    "/usr/local/lib/sjs/modules",
    "~/.local/sjs/modules",
    NULL
};


void sjs__setup_system_module(duk_context* ctx) {
    /* create system module */
    duk_push_object(ctx);
    duk_put_global_string(ctx, "_system");

    duk_get_global_string(ctx, "_system");
    /* -> [ ... system ] */

    /* system.versions */
    {
        char duktape_version[16];

        duk_push_object(ctx);
        /* -> [ ... system obj ] */

        snprintf(duktape_version, sizeof(duktape_version), "v%ld.%ld.%ld",
                                                           DUK_VERSION / 10000,
                                                           DUK_VERSION % 10000 / 100,
                                                           DUK_VERSION % 100);
        duk_push_string(ctx, duktape_version);
        duk_put_prop_string(ctx, -2, "duktape");
        duk_push_lstring(ctx, DUK_GIT_COMMIT, 7);
        duk_put_prop_string(ctx, -2, "duktapeCommit");
        duk_push_string(ctx, sjs_version());
        duk_put_prop_string(ctx, -2, "sjs");
        /* -> [ ... system obj ] */

        duk_put_prop_string(ctx, -2, "versions");
        /* -> [ ... system ] */
    }

    /* system.env */
    {
        duk_push_object(ctx);
        /* -> [ ... system obj ] */

        for (int i = 0; environ[i]; i++) {
            const char* e = environ[i];
            const char* ptr = strrchr(e, '=');
            const char* key;
            const char* value;
            if (!ptr) {
                continue;
            }
            key = e;
            value = ptr + 1;

            duk_push_lstring(ctx, key, (duk_size_t)(ptr - key));
            duk_push_string(ctx, value);
            duk_put_prop(ctx, -3);
        }
        /* -> [ ... system obj ] */

        duk_put_prop_string(ctx, -2, "env");
        /* -> [ ... system ] */
    }

    /* system.path */
    {
        int i = 0;
        duk_push_array(ctx);
        /* -> [ ... system array ] */

        /* path specified by env variable */
        {
            char* sjs_path = getenv("SJS_PATH");
            if (sjs_path) {
                char* token;
                char* saveptr;
                token = strtok_r(sjs_path, ":", &saveptr);
                for (;;) {
                    if (!token) {
                        break;
                    }
                    duk_push_string(ctx, token);
                    duk_put_prop_index(ctx, -2, i);
                    i++;
                    token = strtok_r(NULL, ":", &saveptr);
                }
            }
        }

        /* default search paths */
        {
            for (int j = 0; default_search_paths[j]; j++) {
                duk_push_string(ctx, default_search_paths[j]);
                duk_put_prop_index(ctx, -2, i);
                i++;
            }
        }

        duk_put_prop_string(ctx, -2, "path");
        /* -> [ ... system ] */
    }

    /* system.arch */
    {
        duk_push_string(ctx, DUK_USE_ARCH_STRING);
        duk_put_prop_string(ctx, -2, "arch");
    }

    /* system.platform */
    {
        duk_push_string(ctx, DUK_USE_OS_STRING);
        duk_put_prop_string(ctx, -2, "platform");
    }

    /* system.executable */
    {
        char buf[8192] = {0};
        sjs__executable(buf, sizeof(buf));
        duk_push_string(ctx, buf);
        duk_put_prop_string(ctx, -2, "executable");
    }

    /* system.build */
    {
        duk_push_object(ctx);
        /* -> [ ... system obj ] */

        duk_push_string(ctx, SJS_BUILD_COMPILER);
        duk_put_prop_string(ctx, -2, "compiler");
        duk_push_string(ctx, SJS_BUILD_COMPILER_VERSION);
        duk_put_prop_string(ctx, -2, "compilerVersion");
        duk_push_string(ctx, SJS_BUILD_SYSTEM);
        duk_put_prop_string(ctx, -2, "system");
        duk_push_string(ctx, SJS_BUILD_CFLAGS);
        duk_put_prop_string(ctx, -2, "cflags");
        duk_push_string(ctx, SJS_BUILD_TIMESTAMP);
        duk_put_prop_string(ctx, -2, "timestamp");
        duk_push_string(ctx, SJS_BUILD_TYPE);
        duk_put_prop_string(ctx, -2, "type");

        duk_put_prop_string(ctx, -2, "build");
        /* -> [ ... system ] */
    }

    /* system.endianness */
    {
        const int i = 1;
        if (*(char*)&i == 0) {
            duk_push_string(ctx, "big");
        } else {
            duk_push_string(ctx, "little");
        }
        duk_put_prop_string(ctx, -2, "endianness");
    }

    duk_pop(ctx);
}




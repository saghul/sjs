
#include <assert.h>

#include "duktape.h"
#include "duk_module_node.h"
#include "embedjs.h"

INCBIN(assert,              "modules/assert.js");
INCBIN(codecs,              "modules/codecs.js");
INCBIN(codecs_punycode,     "modules/_codecs_punycode.js");
INCBIN(codecs_utf8,         "modules/_codecs_utf8.js");
INCBIN(console,             "modules/console.js");
INCBIN(errno,               "modules/errno.js");
INCBIN(hash,                "modules/hash.js");
INCBIN(io,                  "modules/io.js");
INCBIN(io_poll,             "modules/_io_poll.js");
INCBIN(io_select,           "modules/_io_select.js");
INCBIN(net,                 "modules/net.js");
INCBIN(os,                  "modules/os.js");
INCBIN(path,                "modules/path.js");
INCBIN(process,             "modules/process.js");
INCBIN(pwd,                 "modules/pwd.js");
INCBIN(random,              "modules/random.js");
INCBIN(system,              "modules/system.js");
INCBIN(time,                "modules/time.js");
INCBIN(utils,               "modules/utils.js");
INCBIN(utils_object,        "modules/_utils_object.js");
INCBIN(utils_unicode,       "modules/_utils_unicode.js");
INCBIN(utils_unorm,         "modules/_utils_unicode_unorm.js");
INCBIN(uuid,                "modules/uuid.js");


void sjs__register_bultins(duk_context* ctx) {
    /*
     * Standalone modules, they have no dependencies.
     */
    duk_push_lstring(ctx, (const char*) sjs__code_codecs_punycode_data, sjs__code_codecs_punycode_size);
    duk_module_node_register_builtin(ctx, "_codecs_punycode");

    duk_push_lstring(ctx, (const char*) sjs__code_codecs_utf8_data, sjs__code_codecs_utf8_size);
    duk_module_node_register_builtin(ctx, "_codecs_utf8");

    duk_push_lstring(ctx, (const char*) sjs__code_codecs_data, sjs__code_codecs_size);
    duk_module_node_register_builtin(ctx, "codecs");

    duk_push_lstring(ctx, (const char*) sjs__code_errno_data, sjs__code_errno_size);
    duk_module_node_register_builtin(ctx, "errno");

    duk_push_lstring(ctx, (const char*) sjs__code_os_data, sjs__code_os_size);
    duk_module_node_register_builtin(ctx, "os");

    duk_push_lstring(ctx, (const char*) sjs__code_path_data, sjs__code_path_size);
    duk_module_node_register_builtin(ctx, "path");

    duk_push_lstring(ctx, (const char*) sjs__code_pwd_data, sjs__code_pwd_size);
    duk_module_node_register_builtin(ctx, "pwd");

    duk_push_lstring(ctx, (const char*) sjs__code_utils_object_data, sjs__code_utils_object_size);
    duk_module_node_register_builtin(ctx, "_utils_object");

    duk_push_lstring(ctx, (const char*) sjs__code_utils_unorm_data, sjs__code_utils_unorm_size);
    duk_module_node_register_builtin(ctx, "_utils_unicode_unorm");

    duk_push_lstring(ctx, (const char*) sjs__code_utils_unicode_data, sjs__code_utils_unicode_size);
    duk_module_node_register_builtin(ctx, "_utils_unicode");

    duk_push_lstring(ctx, (const char*) sjs__code_utils_data, sjs__code_utils_size);
    duk_module_node_register_builtin(ctx, "utils");

    duk_push_lstring(ctx, (const char*) sjs__code_uuid_data, sjs__code_uuid_size);
    duk_module_node_register_builtin(ctx, "uuid");

    /*
     * Dependent modules. These depend on some of the above.
     */
    duk_push_lstring(ctx, (const char*) sjs__code_assert_data, sjs__code_assert_size);
    duk_module_node_register_builtin(ctx, "assert");

    duk_push_lstring(ctx, (const char*) sjs__code_hash_data, sjs__code_hash_size);
    duk_module_node_register_builtin(ctx, "hash");

    duk_push_lstring(ctx, (const char*) sjs__code_io_select_data, sjs__code_io_select_size);
    duk_module_node_register_builtin(ctx, "_io_select");

    duk_push_lstring(ctx, (const char*) sjs__code_io_poll_data, sjs__code_io_poll_size);
    duk_module_node_register_builtin(ctx, "_io_poll");

    duk_push_lstring(ctx, (const char*) sjs__code_io_data, sjs__code_io_size);
    duk_module_node_register_builtin(ctx, "io");

    duk_push_lstring(ctx, (const char*) sjs__code_net_data, sjs__code_net_size);
    duk_module_node_register_builtin(ctx, "net");

    duk_push_lstring(ctx, (const char*) sjs__code_random_data, sjs__code_random_size);
    duk_module_node_register_builtin(ctx, "random");

    /*
     * Dependent modules. These depend on some of the above.
     */
    duk_push_lstring(ctx, (const char*) sjs__code_time_data, sjs__code_time_size);
    duk_module_node_register_builtin(ctx, "time");

    duk_push_lstring(ctx, (const char*) sjs__code_system_data, sjs__code_system_size);
    duk_module_node_register_builtin(ctx, "system");

    /*
     * Dependent modules. These depend on some of the above.
     */
    duk_push_lstring(ctx, (const char*) sjs__code_console_data, sjs__code_console_size);
    duk_module_node_register_builtin(ctx, "console");

    duk_push_lstring(ctx, (const char*) sjs__code_process_data, sjs__code_process_size);
    duk_module_node_register_builtin(ctx, "process");
}


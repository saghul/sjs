
#ifndef SJS_BINDING_H
#define SJS_BINDING_H

#include "internal.h"

void sjs__setup_binding(duk_context* ctx);
void sjs__register_binding(duk_context* ctx,
                           const char* id,
                           const duk_function_list_entry functions[],
                           const duk_number_list_entry constants[]);

/* current bindings */
void sjs__binding_errno_init(duk_context* ctx);
void sjs__binding_gai_init(duk_context* ctx);
void sjs__binding_hash_init(duk_context* ctx);
void sjs__binding_io_init(duk_context* ctx);
void sjs__binding_os_init(duk_context* ctx);
void sjs__binding_path_init(duk_context* ctx);
void sjs__binding_poll_init(duk_context* ctx);
void sjs__binding_pwd_init(duk_context* ctx);
void sjs__binding_random_init(duk_context* ctx);
void sjs__binding_select_init(duk_context* ctx);
void sjs__binding_socket_init(duk_context* ctx);

#endif

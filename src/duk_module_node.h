#if !defined(DUK_MODULE_NODE_H_INCLUDED)
#define DUK_MODULE_NODE_H_INCLUDED

#include "duktape.h"


extern void duk_module_node_init(duk_context *ctx);
extern duk_ret_t duk_module_node_peval_file(duk_context *ctx, const char* filename, int main);
extern void duk_module_node_register_builtin(duk_context *ctx, const char* id);

#endif  /* DUK_MODULE_NODE_H_INCLUDED */


#ifndef SJS_VM_H
#define SJS_VM_H

#include "duktape.h"

/* opaque type */
typedef struct sjs_vm_t sjs_vm_t;

DUK_EXTERNAL_DECL sjs_vm_t* sjs_vm_create(void);
DUK_EXTERNAL_DECL void sjs_vm_destroy(sjs_vm_t* vm);
DUK_EXTERNAL_DECL void sjs_vm_setup_args(sjs_vm_t* vm, int argc, char* argv[]);
DUK_EXTERNAL_DECL duk_context* sjs_vm_get_duk_ctx(sjs_vm_t* vm);

#endif

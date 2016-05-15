
#ifndef SJS_VM_H
#define SJS_VM_H

#include <errno.h>
#include <stdbool.h>

#include "duktape.h"


/* opaque type */
typedef struct sjs_vm_t sjs_vm_t;

DUK_EXTERNAL_DECL sjs_vm_t* sjs_vm_create(void);
DUK_EXTERNAL_DECL void sjs_vm_destroy(sjs_vm_t* vm);
DUK_EXTERNAL_DECL void sjs_vm_setup_args(sjs_vm_t* vm, int argc, char* argv[]);
DUK_EXTERNAL_DECL duk_context* sjs_vm_get_duk_ctx(sjs_vm_t* vm);
DUK_EXTERNAL_DECL int sjs_vm_eval_code(const sjs_vm_t* vm,
                                       const char* filename,
                                       const char* code,
                                       size_t len,
                                       FILE* foutput,
                                       FILE* ferror,
                                       bool use_strict);
DUK_EXTERNAL_DECL int sjs_vm_eval_file(const sjs_vm_t* vm,
                                       const char* filename,
                                       FILE* foutput,
                                       FILE* ferror,
                                       bool use_strict);

DUK_EXTERNAL_DECL int sjs_path_normalize(const char* path, char* normalized_path, size_t normalized_path_len);
DUK_EXTERNAL_DECL uint64_t sjs_time_hrtime(void);


#define SJS_THROW_ERRNO_ERROR2(x)                                                           \
    do {                                                                                    \
        int err = x;                                                                        \
        duk_push_error_object(ctx, DUK_ERR_ERROR, "[errno %d] %s", (err), strerror((err))); \
        duk_push_int(ctx, (err));                                                           \
        duk_put_prop_string(ctx, -2, "errno");                                              \
        duk_throw(ctx);                                                                     \
    } while (0)                                                                             \


#define SJS_THROW_ERRNO_ERROR()     \
    SJS_THROW_ERRNO_ERROR2(errno)


#endif

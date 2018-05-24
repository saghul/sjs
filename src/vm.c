
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "duktape.h"
#include "duk_module_node.h"
#include "vm.h"
#include "binding.h"
#include "internal.h"

#include "bootstrap.h"
#include "polyfill.h"


struct sjs_vm_t {
    duk_context* ctx;
};


static int sjs__compile_execute(duk_context *ctx, void* udata) {
    const char *code;
    duk_size_t len;

    (void) udata;

    /* [ ... code filename ] */

    code = duk_require_lstring(ctx, -2, &len);
    duk_compile_lstring_filename(ctx, 0, code, len);

    /* [ ... code function ] */

    duk_push_global_object(ctx);  /* 'this' binding */
    duk_call_method(ctx, 0);

    return 1;    /* either the result or error are on the stack top */
}


static int sjs__get_error_stack(duk_context *ctx, void* udata) {
    (void) udata;

    if (!duk_is_object(ctx, -1)) {
        return 1;
    }

    if (!duk_has_prop_string(ctx, -1, "stack")) {
        return 1;
    }

    if (!duk_is_error(ctx, -1)) {
        /* Not an Error instance, don't read "stack" */
        return 1;
    }

    duk_get_prop_string(ctx, -1, "stack");  /* caller coerces */
    duk_remove(ctx, -2);
    return 1;
}


static void sjs__dump_result(duk_context* ctx, int r, FILE* foutput, FILE* ferror) {
    if (r != DUK_EXEC_SUCCESS) {
        if (ferror) {
            duk_safe_call(ctx, sjs__get_error_stack, NULL, 1 /*nargs*/, 1 /*nrets*/);
            fprintf(ferror, "%s\n", duk_safe_to_string(ctx, -1));
            fflush(ferror);
        }
    } else {
        if (foutput) {
            /* TODO: make this optional with a parameter? */
            /* beautify output */
            duk_eval_string(ctx, "(function (v) {\n"
                                 "    try {\n"
                                 "        return Duktape.enc('jx', v, null, 4);\n"
                                 "    } catch (e) {\n"
                                 "        return ''+v;\n"
                                 "    }\n"
                                 "})");
            duk_insert(ctx, -2);
            duk_call(ctx, 1);

            fprintf(foutput, "= %s\n", duk_safe_to_string(ctx, -1));
            fflush(foutput);
        }
    }

    duk_pop(ctx);
}


ssize_t sjs__file_read(const char* path, char** data) {
    FILE* f;
    int fd;
    struct stat st;
    size_t fsize;

    *data = NULL;

    f = fopen(path, "rb");
    if (!f) {
        return -errno;
    }

    fd = fileno(f);
    assert(fd != -1);

    if (fstat(fd, &st) < 0) {
        fclose(f);
        return -errno;
    }

    if (S_ISDIR(st.st_mode)) {
        fclose(f);
        return -EISDIR;
    }

    fsize = st.st_size;

    *data = malloc(fsize);
    if (!*data) {
        fclose(f);
        return -ENOMEM;
    }

    fread(*data, 1, fsize, f);
    if (ferror(f)) {
        fclose(f);
        free(*data);
        return -EIO;
    }

    /* remove shebang if present */
    if (strncmp(*data, "#!", 2) == 0) {
        memcpy((void*) *data, "//", 2);
    }

    fclose(f);
    return fsize;
}


DUK_EXTERNAL sjs_vm_t* sjs_vm_create(void) {
    sjs_vm_t* vm;
    vm = calloc(1, sizeof(*vm));
    assert(vm);

    vm->ctx = duk_create_heap(NULL,                     /* alloc function */
                              NULL,                     /* realloc function */
                              NULL,                     /* free function */
                              (void*) vm,               /* user data */
                              NULL                      /* fatal error handler (set in duk_config.h) */
                             );
    assert(vm->ctx != NULL);
    assert(sjs_vm_get_vm(vm->ctx) == vm);

    /* setup builtin modules */
    sjs__setup_system_module(vm->ctx);
    sjs__setup_commonjs(vm->ctx);
    sjs__setup_binding(vm->ctx);

    /* setup polyfill */
    duk_eval_lstring_noresult(vm->ctx, (const char*) sjs__code_polyfill_data, sjs__code_polyfill_size);

    /* register builtin modules */
    sjs__register_bultins(vm->ctx);

    /* run bootstrap code */
    duk_eval_lstring_noresult(vm->ctx, (const char*) sjs__code_bootstrap_data, sjs__code_bootstrap_size);

    return vm;
}


DUK_EXTERNAL void sjs_vm_destroy(sjs_vm_t* vm) {
    assert(vm);

    duk_destroy_heap(vm->ctx);
    vm->ctx = NULL;
    free(vm);
}


DUK_EXTERNAL void sjs_vm_setup_args(sjs_vm_t* vm, int argc, char* argv[]) {
    assert(vm);

    duk_context* ctx = vm->ctx;

    duk_get_global_string(ctx, "_system");

    /* system.argv */
    {
        duk_push_array(ctx);
        /* -> [ ... system array ] */

        for (int i = 0; i < argc; i++) {
            duk_push_string(ctx, argv[i]);
            duk_put_prop_index(ctx, -2, i);
        }

        duk_put_prop_string(ctx, -2, "argv");
        /* -> [ ... system ] */
    }

    duk_pop(ctx);
}


DUK_EXTERNAL int sjs_vm_eval_code_global(const sjs_vm_t* vm,
                                         const char* filename,
                                         const char* code,
                                         size_t len,
                                         FILE* foutput,
                                         FILE* ferror) {
    int r;

    assert(vm);
    duk_context* ctx = vm->ctx;

    duk_push_lstring(ctx, code, len);
    duk_push_string(ctx, filename);
    r = duk_safe_call(ctx, sjs__compile_execute, NULL, 2 /*nargs*/, 1 /*nret*/);

    sjs__dump_result(ctx, r, foutput, ferror);

    return r;
}


DUK_EXTERNAL int sjs_vm_eval_code(const sjs_vm_t* vm,
                                  const char* filename,
                                  const char* code,
                                  size_t len,
                                  FILE* foutput,
                                  FILE* ferror) {
    int r;

    assert(vm);
    duk_context* ctx = vm->ctx;

    duk_push_lstring(ctx, code, len);
    r = duk_module_node_peval_file(ctx, filename, 0);

    sjs__dump_result(ctx, r, foutput, ferror);

    return r;
}


DUK_EXTERNAL int sjs_vm_eval_file(const sjs_vm_t* vm,
                                  const char* filename,
                                  FILE* foutput,
                                  FILE* ferror) {
    int r;
    char* data;
    char path[SJS_PATH_MAX];

    r = sjs__path_normalize(filename, path, sizeof(path));
    if (r < 0) {
        if (ferror) {
            fprintf(ferror, "sjs: cannot open file '%s': [Errno %d] %s\n", filename, -r, strerror(-r));
            fflush(ferror);
        }
        return r;
    }
    r = sjs__file_read(path, &data);
    if (r < 0) {
        if (ferror) {
            fprintf(ferror, "sjs: cannot open file '%s': [Errno %d] %s\n", filename, -r, strerror(-r));
            fflush(ferror);
        }
        return r;
    } else if (r == 0) {
        /* also return in case of a 0 sized file */
        free(data);
        return r;
    } else {
        duk_context* ctx;
        ctx = vm->ctx;
        duk_push_lstring(ctx, data, r);
        free(data);
        r = duk_module_node_peval_file(ctx, path, 1);
        sjs__dump_result(ctx, r, foutput, ferror);
        return r;
    }
}


DUK_EXTERNAL duk_context* sjs_vm_get_duk_ctx(sjs_vm_t* vm) {
    assert(vm);
    return vm->ctx;
}


DUK_EXTERNAL sjs_vm_t* sjs_vm_get_vm(duk_context* ctx) {
    duk_memory_functions funcs;
    assert(ctx);
    duk_get_memory_functions(ctx, &funcs);
    return funcs.udata;
}


DUK_EXTERNAL int sjs_path_expanduser(const char* path, char* normalized_path, size_t normalized_path_len) {
    return sjs__path_expanduser(path, normalized_path, normalized_path_len);
}


DUK_EXTERNAL int sjs_path_normalize(const char* path, char* normalized_path, size_t normalized_path_len) {
    return sjs__path_normalize(path, normalized_path, normalized_path_len);
}


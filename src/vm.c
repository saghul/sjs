
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "duktape.h"
#include "vm.h"
#include "internal.h"
#include "es6shim.h"


struct sjs_vm_t {
    duk_context* ctx;
};


/* generated from src/bootstrap.js */
static const char SJS__BOOTSTRAP_CODE[] = {
    0,
};


static void sjs__duk_fatal_handler(duk_context *ctx, duk_errcode_t code, const char *msg) {
    (void) ctx;
    fprintf(stderr, "FATAL %ld: %s\n", (long) code, (const char *) (msg ? msg : "null"));
    fflush(stderr);
    abort();
}


static void sjs__setup_modsearch(sjs_vm_t* vm) {
    duk_context* ctx = vm->ctx;

    duk_get_global_string(ctx, "Duktape");
    duk_push_string(ctx, "modSearch");
    duk_push_c_function(ctx, sjs__modsearch, 4 /* nargs */);
    duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE |
                          DUK_DEFPROP_CLEAR_WRITABLE |
                          DUK_DEFPROP_CLEAR_ENUMERABLE |
                          DUK_DEFPROP_CLEAR_CONFIGURABLE);
    duk_pop(ctx);
}


static void sjs__setup_global_module(sjs_vm_t* vm) {
    duk_context* ctx = vm->ctx;

    duk_push_global_object(ctx);
    duk_put_global_string(ctx, "global");
}


static int sjs__compile_execute(duk_context *ctx) {
    const char *code;
    const char* filename;
    duk_size_t len;
    bool use_strict;
    int flags;

    /* [ ... use_strict code len filename ] */

    use_strict = duk_require_boolean(ctx, -4);
    code = duk_require_pointer(ctx, -3);
    len = duk_require_uint(ctx, -2);
    filename = duk_require_string(ctx, -1);

    flags = 0;
    if (use_strict) {
        flags |= DUK_COMPILE_STRICT;
    }

    /* remove shebang if present */
    if (strncmp(code, "#!", 2) == 0) {
        memcpy((void*) code, "//", 2);
    }

    duk_compile_lstring_filename(ctx, flags, code, len);

    /* [ ... use_strict code len function ] */

    duk_push_global_object(ctx);  /* 'this' binding */
    duk_push_string(ctx, filename);
    duk_put_prop_string(ctx, -2, "__file__");
    duk_call_method(ctx, 0);

    return 1;    /* either the result or error are on the stack top */
}


static int sjs__get_error_stack(duk_context *ctx) {
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

    fread(*data, fsize, 1, f);
    if (ferror(f)) {
        fclose(f);
        free(*data);
        return -EIO;
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
                              NULL,                     /* user data */
                              sjs__duk_fatal_handler    /* fatal error handler */
                             );
    assert(vm->ctx != NULL);

    /* setup builtin modules */
    sjs__setup_system_module(vm->ctx);
    sjs__setup_global_module(vm);
    sjs__setup_modsearch(vm);

    /* setup es6 shim */
    duk_eval_string_noresult(vm->ctx, sjs__es6shim_src);

    /* run bootstrap code */
    duk_eval_string_noresult(vm->ctx, SJS__BOOTSTRAP_CODE);

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


DUK_EXTERNAL int sjs_vm_eval_code(const sjs_vm_t* vm,
                                  const char* filename,
                                  const char* code,
                                  size_t len,
                                  FILE* foutput,
                                  FILE* ferror,
                                  bool use_strict) {
    int r;

    assert(vm);
    duk_context* ctx = vm->ctx;

    duk_push_boolean(ctx, use_strict);
    duk_push_pointer(ctx, (void *) code);
    duk_push_uint(ctx, len);
    duk_push_string(ctx, filename);

    r = duk_safe_call(ctx, sjs__compile_execute, 4 /*nargs*/, 1 /*nret*/);
    if (r != DUK_EXEC_SUCCESS) {
        if (ferror) {
            duk_safe_call(ctx, sjs__get_error_stack, 1 /*nargs*/, 1 /*nrets*/);
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
                                 "        return String(v);\n"
                                 "    }\n"
                                 "})");
            duk_insert(ctx, -2);
            duk_call(ctx, 1);

            fprintf(foutput, "= %s\n", duk_safe_to_string(ctx, -1));
            fflush(foutput);
        }
    }

    duk_pop(ctx);
    return r;
}


DUK_EXTERNAL int sjs_vm_eval_file(const sjs_vm_t* vm,
                                  const char* filename,
                                  FILE* foutput,
                                  FILE* ferror,
                                  bool use_strict) {
    int r;
    char* data;
    char path[8192];

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
        r = sjs_vm_eval_code(vm, path, data, r, foutput, ferror, use_strict);
        free(data);
        return r;
    }
}


DUK_EXTERNAL duk_context* sjs_vm_get_duk_ctx(sjs_vm_t* vm) {
    assert(vm);
    return vm->ctx;
}


DUK_EXTERNAL int sjs_path_normalize(const char* path, char* normalized_path, size_t normalized_path_len) {
    return sjs__path_normalize(path, normalized_path, normalized_path_len);
}

DUK_EXTERNAL uint64_t sjs_time_hrtime(void) {
    return sjs__hrtime();
}


#ifndef SJS_INTERNAL_H
#define SJS_INTERNAL_H

#include "duktape.h"


duk_ret_t sjs__modsearch(duk_context* ctx);
void sjs__setup_system_module(duk_context* ctx);
int sjs__path_expanduser(const char* path, char* normalized_path, size_t normalized_path_len);
int sjs__path_normalize(const char* path, char* normalized_path, size_t normalized_path_len);
void sjs__executable(char* buf, size_t size);
uint64_t sjs__hrtime(void);
ssize_t sjs__file_read(const char* path, char** data);

/* string functions borrowed from OpenBSD */
size_t sjs__strlcat(char *dst, const char *src, size_t dsize);
size_t sjs__strlcpy(char *dst, const char *src, size_t dsize);

#endif

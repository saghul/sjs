
#ifndef SJS_INTERNAL_H
#define SJS_INTERNAL_H

#include "duktape.h"


duk_ret_t sjs__modsearch(duk_context* ctx);
int sjs__path_normalize(const char* path, char* normalized_path, size_t normalized_path_len);
void sjs__executable(char* buf, size_t size);

#endif

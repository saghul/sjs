
#ifndef SJS_INTERNAL_H
#define SJS_INTERNAL_H

#include "duktape.h"

#define SJS_PATH_MAX 4096
#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))


void sjs__setup_system_module(duk_context* ctx);
void sjs__setup_commonjs(duk_context* ctx);
void sjs__register_bultins(duk_context* ctx);

int sjs__path_expanduser(const char* path, char* normalized_path, size_t normalized_path_len);
int sjs__path_normalize(const char* path, char* normalized_path, size_t normalized_path_len);
ssize_t sjs__file_read(const char* path, char** data);

/* platform */
void sjs__executable(char* buf, size_t size);
int sjs__cloexec(int fd, int set);
int sjs__nonblock(int fd, int set);
int sjs__close(int fd);

/* string functions borrowed from OpenBSD */
size_t sjs__strlcat(char *dst, const char *src, size_t dsize);
size_t sjs__strlcpy(char *dst, const char *src, size_t dsize);

#endif

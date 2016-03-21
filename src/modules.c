
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "internal.h"


ssize_t sjs__read_file(const char* path, char** data) {
    FILE* f;
    long fsize;

    *data = NULL;

    f = fopen(path, "rb");
    if (!f) {
    	return -errno;
    }

    fseek(f, 0, SEEK_END);
    fsize = ftell(f);
    assert(fsize != -1);
    rewind(f);

    *data = malloc(fsize);
    assert(*data);
    fread(*data, fsize, 1, f);
    fclose(f);

    return fsize;
}

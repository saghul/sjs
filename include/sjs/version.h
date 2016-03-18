
#ifndef SJS_VERSION_H
#define SJS_VERSION_H

#include "duktape.h"


#define SJS_VERSION_MAJOR 0
#define SJS_VERSION_MINOR 0
#define SJS_VERSION_PATCH 1
#define SJS_VERSION_SUFFIX "dev"

DUK_EXTERNAL_DECL const char* sjs_version(void);

#endif


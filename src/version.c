
#include "duktape.h"
#include "version.h"


#define SJS_STRINGIFY(v) SJS_STRINGIFY_HELPER(v)
#define SJS_STRINGIFY_HELPER(v) #v

#define SJS_VERSION_STRING  SJS_STRINGIFY(SJS_VERSION_MAJOR) "." \
                            SJS_STRINGIFY(SJS_VERSION_MINOR) "." \
                            SJS_STRINGIFY(SJS_VERSION_PATCH)     \
			    SJS_VERSION_SUFFIX


DUK_EXTERNAL const char* sjs_version(void) {
    return SJS_VERSION_STRING;
}

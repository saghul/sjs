
#include <mach/mach.h>
#include <mach/mach_time.h>
#include <mach-o/dyld.h> /* _NSGetExecutablePath */
#include <stdlib.h>

#include "../internal.h"


void sjs__executable(char* buf, size_t size) {
    /* realpath(exepath) may be > PATH_MAX so double it to be on the safe side. */
    char abspath[PATH_MAX * 2 + 1];
    char exepath[PATH_MAX + 1];
    uint32_t exepath_size;
    size_t abspath_size;

    exepath_size = sizeof(exepath);
    if (_NSGetExecutablePath(exepath, &exepath_size)) {
        return;
    }

    if (realpath(exepath, abspath) != abspath) {
        return;
    }

    abspath_size = strlen(abspath);
    if (abspath_size == 0) {
        return;
    }

    size -= 1;
    if (size > abspath_size)
        size = abspath_size;

    memcpy(buf, abspath, size);
    buf[size] = '\0';
}


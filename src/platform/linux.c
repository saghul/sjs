
#include <unistd.h>


void sjs__executable(char* buf, size_t size) {
    ssize_t n;

    n = size - 1;
    if (n > 0) {
	n = readlink("/proc/self/exe", buf, n);
    }

    if (n == -1) {
        return;
    }

    buf[n] = '\0';
}


uint64_t sjs__hrtime(void) {
    /* TODO */
    return 0;
}


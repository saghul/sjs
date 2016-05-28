
#ifndef SJS_PLATFORM_H
#define SJS_PLATFORM_H

#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>


 __attribute__((unused))
static int sjs__cloexec(int fd, int set) {
    int r;

    do
        r = ioctl(fd, set ? FIOCLEX : FIONCLEX);
    while (r == -1 && errno == EINTR);

    if (r)
        return -errno;

    return 0;
}


 __attribute__((unused))
static int sjs__nonblock(int fd, int set) {
    int r;

    do
        r = ioctl(fd, FIONBIO, &set);
    while (r == -1 && errno == EINTR);

    if (r)
        return -errno;

    return 0;
}

#endif

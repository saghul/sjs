
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


 __attribute__((unused))
int sjs__close(int fd) {
  int saved_errno;
  int r;

  saved_errno = errno;
  r = close(fd);
  if (r == -1) {
    r = -errno;
    if (r == -EINTR || r == -EINPROGRESS) {
      r = 0;    /* The close is in progress, not an error. */
    }
    errno = saved_errno;
  }

  return r;
}

#endif

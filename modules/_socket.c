
#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <netinet/tcp.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

#include "../src/platform-inl.h"
#include <sjs/sjs.h>


static int sjs__socket(int domain, int type, int protocol) {
    int r;
#ifdef __linux__
    r = socket(domain, type | SOCK_CLOEXEC, protocol);
    if (r < 0) {
        return -errno;
    }
    return r;
#endif
    int fd;
    r = socket(domain, type, protocol);
    if (r < 0) {
        return r;
    }
    fd = r;
    r = sjs__cloexec(fd, 1);
    if (r < 0) {
        sjs__close(fd);
        return r;
    }
    return fd;
}

/*
 * Create a socket. Args:
 * - 0: domain
 * - 1: type
 */
static duk_ret_t sock_socket(duk_context* ctx) {
    int domain;
    int type;
    int r;

    domain = duk_require_int(ctx, 0);
    type = duk_require_int(ctx, 1);

    r = sjs__socket(domain, type, 0);
    if (r < 0) {
        SJS_THROW_ERRNO_ERROR2(-r);
        return -42;    /* control never returns here */
    } else {
        duk_push_int(ctx, r);
        return 1;
    }
}


/*
 * Prse a socket address object at the given index. Returns 0 on success or -1 in case of error, and puts an error
 * object on top of the stack.
 */
static int sock__obj2addr(duk_context* ctx, duk_idx_t addr_idx, struct sockaddr_storage* ss, socklen_t* addrlen) {
    int domain;

    memset(ss, 0, sizeof(*ss));
    *addrlen = 0;

    duk_get_prop_string(ctx, addr_idx, "domain");
    domain = duk_require_int(ctx, -1);
    duk_pop(ctx);

    switch(domain) {
        case AF_INET:
        {
            /* 'address' is an object with 'host' and 'port' */
            struct sockaddr_in* addr4;
            const char* host;
            int port;
            addr4 = (struct sockaddr_in*) ss;
            duk_get_prop_string(ctx, addr_idx, "host");
            host = duk_require_string(ctx, -1);
            duk_get_prop_string(ctx, addr_idx, "port");
            port = duk_require_int(ctx, -1);
            if (inet_pton(AF_INET, host, &(addr4->sin_addr.s_addr)) != 1) {
                duk_pop_2(ctx);
                duk_push_error_object(ctx, DUK_ERR_ERROR, "invalid IP address: %s", host);
                return -1;
            }
            if (port < 0 || port > 0xffff) {
                duk_pop_2(ctx);
                duk_push_error_object(ctx, DUK_ERR_RANGE_ERROR, "invalid port: %d", port);
                return -1;
            }
            addr4->sin_port = htons(port);
            addr4->sin_family = AF_INET;
            *addrlen = sizeof(*addr4);
            duk_pop_2(ctx);
            break;
        }
        case AF_INET6:
        {
            /* 'address' is an object with 'host', 'port', 'flowinfo' and 'scopeid' */
            struct sockaddr_in6* addr6;
            const char* host;
            int port;
            int flowinfo;
            int scopeid;
            addr6 = (struct sockaddr_in6*) ss;
            duk_get_prop_string(ctx, addr_idx, "host");
            host = duk_require_string(ctx, -1);
            duk_get_prop_string(ctx, addr_idx, "port");
            port = duk_require_int(ctx, -1);
            duk_get_prop_string(ctx, addr_idx, "flowinfo");
            flowinfo = duk_require_int(ctx, -1);
            duk_get_prop_string(ctx, addr_idx, "scopeid");
            scopeid = duk_require_int(ctx, -1);
            if (inet_pton(AF_INET6, host, &(addr6->sin6_addr)) != 1) {
                duk_pop_n(ctx, 4);
                duk_push_error_object(ctx, DUK_ERR_ERROR, "invalid IP address: %s", host);
                return -1;
            }
            if (port < 0 || port > 0xffff) {
                duk_pop_n(ctx, 4);
                duk_push_error_object(ctx, DUK_ERR_RANGE_ERROR, "invalid port: %d", port);
                return -1;
            }
            if (flowinfo > 0xfffff) {
                duk_pop_n(ctx, 4);
                duk_push_error_object(ctx, DUK_ERR_RANGE_ERROR, "invalid flowinfo: %d", port);
                return -1;
            }
            addr6->sin6_port = htons(port);
            addr6->sin6_flowinfo = flowinfo;
            addr6->sin6_scope_id = scopeid;
            addr6->sin6_family = AF_INET6;
            *addrlen = sizeof(*addr6);
            duk_pop_n(ctx, 4);
            break;
        }
        case AF_UNIX:
        {
            /* 'address' is a string, representing the path */
            struct sockaddr_un* addru;
            const char* path;
            size_t path_len;
            addru = (struct sockaddr_un*) ss;
            duk_get_prop_string(ctx, addr_idx, "path");
            path = duk_require_lstring(ctx, -1, &path_len);
#ifdef __linux__
            if (path_len > 0 && path[0] == '\0') {
                if (path_len > sizeof(addru->sun_path)) {
                    duk_pop(ctx);
                    duk_push_error_object(ctx, DUK_ERR_RANGE_ERROR, "invalid abstract socket name");
                    return -1;
                }
                memcpy(addru->sun_path, path, path_len);
                *addrlen = offsetof(struct sockaddr_un, sun_path) + path_len;
            } else
#endif
            {
                strncpy(addru->sun_path, path, sizeof(addru->sun_path) - 1);
                addru->sun_path[sizeof(addru->sun_path) - 1] = '\0';
                *addrlen = sizeof(*addru);
            }
            addru->sun_family = AF_UNIX;
            duk_pop(ctx);
            break;
        }
        default:
            abort();
    }
    return 0;
}


/*
 * Create a socket address object out of a struct sockaddr*. The resulting object is at the top of the stack.
 */
static void sock__addr2obj(duk_context* ctx, const struct sockaddr* addr, const socklen_t addrlen) {
    (void) addrlen;  /* avoid compilation warning on OSX */
    switch (addr->sa_family) {
        case AF_INET:
        {
            struct sockaddr_in* addr4;
            char host[INET_ADDRSTRLEN];
            int port;
            addr4 = (struct sockaddr_in*) addr;
            if (inet_ntop(AF_INET, &(addr4->sin_addr), host, sizeof(host)) == NULL) {
            	duk_push_undefined(ctx);
            	return;
            }
            port = ntohs(addr4->sin_port);
            duk_push_object(ctx);
            duk_push_string(ctx, host);
            duk_put_prop_string(ctx, -2, "host");
            duk_push_int(ctx, port);
            duk_put_prop_string(ctx, -2, "port");
            break;
        }
        case AF_INET6:
        {
            struct sockaddr_in6* addr6;
            char host[INET6_ADDRSTRLEN];
            int port;
            addr6 = (struct sockaddr_in6*) addr;
            if (inet_ntop(AF_INET6, &(addr6->sin6_addr), host, sizeof(host)) == NULL) {
            	duk_push_undefined(ctx);
            	return;
            }
            port = ntohs(addr6->sin6_port);
            duk_push_object(ctx);
            duk_push_string(ctx, host);
            duk_put_prop_string(ctx, -2, "host");
            duk_push_int(ctx, port);
            duk_put_prop_string(ctx, -2, "port");
            duk_push_int(ctx, addr6->sin6_flowinfo);
            duk_put_prop_string(ctx, -2, "flowinfo");
            duk_push_int(ctx, addr6->sin6_scope_id);
            duk_put_prop_string(ctx, -2, "scopeid");
            break;
        }
        case AF_UNIX:
        {
            struct sockaddr_un* addru;
            size_t len;
            addru = (struct sockaddr_un*) addr;
#ifdef __linux__
            if (addru->sun_path[0] == '\0') {
                /* Linux abstract namespace */
                len = addrlen - offsetof(struct sockaddr_un, sun_path);
            }
            else
#endif
            {
                len = strlen(addru->sun_path);
            }
            duk_push_lstring(ctx, addru->sun_path, len);
            break;
        }
        default:
            abort();
    }
}


/*
 * Bind the socket. Args:
 * - 0: fd
 * - 1: address object
 */
static duk_ret_t sock_bind(duk_context* ctx) {
    int r;
    int fd;
    struct sockaddr_storage ss;
    socklen_t addrlen;

    fd = duk_require_int(ctx, 0);

    assert(fd >= 0);

    if (sock__obj2addr(ctx, 1, &ss, &addrlen) != 0) {
        /* the stack top contains the error object */
        duk_throw(ctx);
        return -42;    /* control never returns here */
    }

    assert(addrlen > 0);

    r = bind(fd, (const struct sockaddr*) &ss, addrlen);
    if (r != 0) {
        SJS_THROW_ERRNO_ERROR();
        return -42;    /* control never returns here */
    } else {
        duk_push_undefined(ctx);
        return 1;
    }
}


/*
 * Connect the socket. Args:
 * - 0: fd
 * - 1: address object
 */
static duk_ret_t sock_connect(duk_context* ctx) {
    int r;
    int fd;
    struct sockaddr_storage ss;
    socklen_t addrlen;

    fd = duk_require_int(ctx, 0);

    assert(fd >= 0);

    if (sock__obj2addr(ctx, 1, &ss, &addrlen) != 0) {
        /* the stack top contains the error object */
        duk_throw(ctx);
        return -42;    /* control never returns here */
    }

    assert(addrlen > 0);

    r = connect(fd, (const struct sockaddr*) &ss, addrlen);
    if (r != 0) {
        SJS_THROW_ERRNO_ERROR();
        return -42;    /* control never returns here */
    } else {
        duk_push_undefined(ctx);
        return 1;
    }
}


typedef int (*sjs__socknamefunc)(int, struct sockaddr*, socklen_t*);

static duk_ret_t sock__getsockpeername(duk_context* ctx, sjs__socknamefunc func) {
    int r;
    int fd;
    struct sockaddr_storage ss;
    socklen_t len;

    fd = duk_require_int(ctx, 0);
    len = sizeof(ss);
    r = func(fd, (struct sockaddr*) &ss, &len);
    if (r != 0) {
        SJS_THROW_ERRNO_ERROR();
        return -42;    /* control never returns here */
    }

    sock__addr2obj(ctx, (const struct sockaddr*) &ss, len);

    return 1;
}


static duk_ret_t sock_getsockname(duk_context* ctx) {
    return sock__getsockpeername(ctx, getsockname);
}


static duk_ret_t sock_getpeername(duk_context* ctx) {
    return sock__getsockpeername(ctx, getpeername);
}


/*
 * Shutdown the socket. Args:
 * - 0: fd
 * - 1: how
 */
static duk_ret_t sock_shutdown(duk_context* ctx) {
    int fd;
    int how;
    int r;

    fd = duk_require_int(ctx, 0);
    how = duk_require_int(ctx, 1);

    r = shutdown(fd, how);
    if (r != 0) {
        SJS_THROW_ERRNO_ERROR();
        return -42;    /* control never returns here */
    }

    duk_push_undefined(ctx);
    return 1;
}


/*
 * Put the socket in listening mode. Args:
 * - 0: fd
 * - 1: backlog
 */
static duk_ret_t sock_listen(duk_context* ctx) {
    int fd;
    int backlog;
    int r;

    fd = duk_require_int(ctx, 0);
    backlog = duk_require_int(ctx, 1);

    r = listen(fd, backlog);
    if (r != 0) {
        SJS_THROW_ERRNO_ERROR();
        return -42;    /* control never returns here */
    }

    duk_push_undefined(ctx);
    return 1;
}


static int sjs__accept(int sockfd) {
    int r;
#if defined(__linux__)
    r = accept4(sockfd, NULL, NULL, SOCK_CLOEXEC);
    if (r < 0) {
        return -errno;
    }
    return r;
#endif
    int fd;
    r = accept(sockfd, NULL, NULL);
    if (r < 0) {
        return -errno;
    }
    fd = r;
    r = sjs__cloexec(fd, 1);
    if (r < 0) {
        sjs__close(fd);
        return r;
    }
    return fd;
}

/*
 * Accept an incoming connection. Args:
 * - 0: fd
 */
static duk_ret_t sock_accept(duk_context* ctx) {
    int fd;
    int r;

    fd = duk_require_int(ctx, 0);

    r = sjs__accept(fd);
    if (r < 0) {
        SJS_THROW_ERRNO_ERROR2(-r);
        return -42;    /* control never returns here */
    }

    duk_push_int(ctx, r);
    return 1;
}


/*
 * Read data from a socket. Args:
 * - 0: fd
 * - 1: nrecv (a number or a Buffer-ish object)
 */
static duk_ret_t sock_recv(duk_context* ctx) {
    int fd;
    ssize_t r;
    size_t nrecv;
    char* buf;
    char* alloc_buf = NULL;

    fd = duk_require_int(ctx, 0);
    if (duk_is_number(ctx, 1)) {
        nrecv = duk_require_int(ctx, 1);
        alloc_buf = malloc(nrecv);
        if (!alloc_buf) {
            SJS_THROW_ERRNO_ERROR2(ENOMEM);
            return -42;    /* control never returns here */
        }
        buf = alloc_buf;
    } else {
        buf = duk_require_buffer_data(ctx, 1, &nrecv);
        if (buf == NULL || nrecv == 0) {
            duk_error(ctx, DUK_ERR_TYPE_ERROR, "invalid buffer");
            return -42;    /* control never returns here */
        }
    }

    r = recv(fd, buf, nrecv, 0);
    if (r < 0) {
        free(alloc_buf);
        SJS_THROW_ERRNO_ERROR();
        return -42;    /* control never returns here */
    } else {
        if (alloc_buf) {
            /* return the string we read */
            duk_push_lstring(ctx, buf, r);
        } else {
            /* the data was written to the buffer, return how much we read */
            duk_push_int(ctx, r);
        }

        free(alloc_buf);
        return 1;
    }
}


/*
 * Write data to a socket. Args:
 * - 0: fd
 * - 1: data
 */
static duk_ret_t sock_send(duk_context* ctx) {
    int fd;
    ssize_t r;
    size_t len;
    const char* buf;

    fd = duk_require_int(ctx, 0);
    if (duk_is_string(ctx, 1)) {
        buf = duk_require_lstring(ctx, 1, &len);
    } else {
        buf = duk_require_buffer_data(ctx, 1, &len);
    }

    r = send(fd, buf, len, 0);
    if (r < 0) {
        SJS_THROW_ERRNO_ERROR();
        return -42;    /* control never returns here */
    } else {
        duk_push_int(ctx, r);
    }

    return 1;
}


/*
 * Receive data from a socket. Returns an object with
 * 'data' and 'address'. Args:
 * - 0: fd
 * - 1: nrecv (a number or a Buffer-ish object)
 */
static duk_ret_t sock_recvfrom(duk_context* ctx) {
    int fd;
    ssize_t r;
    size_t nrecv;
    char* buf;
    struct sockaddr_storage ss;
    socklen_t addrlen;
    char* alloc_buf = NULL;

    addrlen = sizeof(ss);
    fd = duk_require_int(ctx, 0);
    if (duk_is_number(ctx, 1)) {
        nrecv = duk_require_int(ctx, 1);
        alloc_buf = malloc(nrecv);
        if (!alloc_buf) {
            SJS_THROW_ERRNO_ERROR2(ENOMEM);
            return -42;    /* control never returns here */
        }
        buf = alloc_buf;
    } else {
        buf = duk_require_buffer_data(ctx, 1, &nrecv);
        if (buf == NULL || nrecv == 0) {
            duk_error(ctx, DUK_ERR_TYPE_ERROR, "invalid buffer");
            return -42;    /* control never returns here */
        }
    }

    r = recvfrom(fd, buf, nrecv, 0, (struct sockaddr*) &ss, &addrlen);
    if (r < 0) {
        free(alloc_buf);
        SJS_THROW_ERRNO_ERROR();
        return -42;    /* control never returns here */
    } else {
        duk_push_object(ctx);
        if (alloc_buf) {
            duk_push_lstring(ctx, buf, r);
            duk_put_prop_string(ctx, -2, "data");
        } else {
            duk_push_int(ctx, r);
            duk_put_prop_string(ctx, -2, "nrecv");
        }
        if (addrlen > 0) {
            sock__addr2obj(ctx, (const struct sockaddr*) &ss, addrlen);
        } else {
            duk_push_undefined(ctx);
        }
        duk_put_prop_string(ctx, -2, "address");

        free(alloc_buf);
        return 1;
    }
}


/*
 * Send data to a unconnected socket. Args:
 * - 0: fd
 * - 1: data
 * - 2: address
 */
static duk_ret_t sock_sendto(duk_context* ctx) {
    int fd;
    ssize_t r;
    size_t len;
    const char* buf;
    struct sockaddr_storage ss;
    socklen_t addrlen;

    fd = duk_require_int(ctx, 0);
    if (duk_is_string(ctx, 1)) {
        buf = duk_require_lstring(ctx, 1, &len);
    } else {
        buf = duk_require_buffer_data(ctx, 1, &len);
    }

    if (sock__obj2addr(ctx, 2, &ss, &addrlen) != 0) {
        /* the stack top contains the error object */
        duk_throw(ctx);
        return -42;    /* control never returns here */
    }

    r = sendto(fd, buf, len, 0, (const struct sockaddr*) &ss, addrlen);
    if (r < 0) {
        SJS_THROW_ERRNO_ERROR();
        return -42;    /* control never returns here */
    } else {
        duk_push_int(ctx, r);
    }

    return 1;
}


/*
 * Convert an IP address into it's packed network format. Args:
 * - 0: domain
 * - 1: address
 */
static duk_ret_t sock_inet_pton(duk_context* ctx) {
    int domain;
    const char* addr;
    char buf[sizeof(struct in6_addr)];

    domain = duk_require_int(ctx, 0);
    addr = duk_require_string(ctx, 1);

    if (inet_pton(domain, addr, buf) != 1) {
        duk_push_undefined(ctx);
    } else if (domain == AF_INET) {
        duk_push_lstring(ctx, buf, sizeof(struct in_addr));
    } else if (domain == AF_INET6) {
        duk_push_lstring(ctx, buf, sizeof(struct in6_addr));
    } else {
        abort();
    }

    return 1;
}


/*
 * Set a socket option
 * - 0: fd
 * - 1: level
 * - 2: option
 * - 3: value
 */
static duk_ret_t sock_setsockopt(duk_context* ctx) {
    int r, fd, level, option;

    fd = duk_require_int(ctx, 0);
    level = duk_require_int(ctx, 1);
    option = duk_require_int(ctx, 2);

    if (duk_is_number(ctx, 3)) {
        int flag;
        flag = duk_require_int(ctx, 3);
        r = setsockopt(fd, level, option, (char*) &flag, sizeof(flag));
    } else {
        const char* opt;
        size_t len;
        opt = duk_require_lstring(ctx, 3, &len);
        r = setsockopt(fd, level, option, opt, len);
    }

    if (r) {
        SJS_THROW_ERRNO_ERROR();
        return -42;    /* control never returns here */
    }

    duk_push_undefined(ctx);
    return 1;
}


/*
 * Get a socket option
 * - 0: fd
 * - 1: level
 * - 2: option
 * - 3: size (for non integer options)
 */
static duk_ret_t sock_getsockopt(duk_context* ctx) {
    int r, fd, level, option;
    socklen_t optlen;

    fd = duk_require_int(ctx, 0);
    level = duk_require_int(ctx, 1);
    option = duk_require_int(ctx, 2);

    if (duk_is_undefined(ctx, 3)) {
        int value;
        optlen = sizeof(value);
        r = getsockopt(fd, level, option, (void*) &value, &optlen);
        if (r) {
            SJS_THROW_ERRNO_ERROR();
            return -42;    /* control never returns here */
        }
        duk_push_int(ctx, value);
        return 1;
    } else {
        char opt[1024];
        optlen = duk_require_uint(ctx, 3);
        if (optlen > sizeof(opt)) {
            duk_error(ctx, DUK_ERR_TYPE_ERROR, "option size");
            return -42;    /* control never returns here */
        }
        r = getsockopt(fd, level, option, opt, &optlen);
        if (r) {
            SJS_THROW_ERRNO_ERROR();
            return -42;    /* control never returns here */
        }
        duk_push_lstring(ctx, opt, optlen);
        return 1;
    }
}


static int sjs__socketpair(int domain, int type, int protocol, int sv[2]) {
    int r;
#if defined(__linux__)
    r = socketpair(domain, type | SOCK_CLOEXEC, protocol, sv);
    if (r < 0) {
        return -errno;
    }
    return r;
#endif
    r = socketpair(domain, type, protocol, sv);
    if (r < 0) {
        return -errno;
    }
    r = sjs__cloexec(sv[0], 1);
    if (r < 0) {
        goto error;
    }
    r = sjs__cloexec(sv[1], 1);
    if (r < 0) {
        goto error;
    }
    return r;
error:
    sjs__close(sv[0]);
    sjs__close(sv[1]);
    sv[0] = -1;
    sv[1] = -1;
    return r;
}

/*
 * Create a socketpair. Args:
 * - 0: domain
 * - 1: type
 */
static duk_ret_t sock_socketpair(duk_context* ctx) {
    int domain, type, r;
    int fds[2];

    domain = duk_require_int(ctx, 0);
    type = duk_require_int(ctx, 1);

    r = sjs__socketpair(domain, type, 0, fds);
    if (r < 0) {
        SJS_THROW_ERRNO_ERROR2(-r);
        return -42;    /* control never returns here */
    } else {
        duk_push_array(ctx);
        duk_push_int(ctx, fds[0]);
        duk_put_prop_index(ctx, -2, 0);
        duk_push_int(ctx, fds[1]);
        duk_put_prop_index(ctx, -2, 1);
        return 1;
    }
}


#define X(name) {#name, name}
static const duk_number_list_entry module_consts[] = {
    /* socket domain */
    X(AF_INET),
    X(AF_INET6),
    X(AF_UNIX),
    /* socket type */
    X(SOCK_DGRAM),
    X(SOCK_STREAM),
    /* shutdown how */
    X(SHUT_RD),
    X(SHUT_WR),
    X(SHUT_RDWR),
    /* level for getsockopt/setsockopt */
    X(SOL_SOCKET),
    X(IPPROTO_TCP),
    X(IPPROTO_IP),
    X(IPPROTO_IPV6),
    /* options for getsockopt/setsockopt */
    X(SO_DEBUG),
    X(SO_REUSEADDR),
    X(SO_REUSEPORT),
    X(SO_KEEPALIVE),
    X(SO_DONTROUTE),
    X(SO_LINGER),
    X(SO_BROADCAST),
    X(SO_OOBINLINE),
    X(SO_SNDBUF),
    X(SO_RCVBUF),
    X(SO_ERROR),
    X(TCP_NODELAY),
    X(TCP_KEEPCNT),
    X(TCP_KEEPINTVL),
#ifdef TCP_KEEPALIVE
    X(TCP_KEEPALIVE),
#endif
#ifdef TCP_KEEPIDLE
    X(TCP_KEEPIDLE),
#endif
#ifdef TCP_CORK
    X(TCP_CORK),
#endif
    X(IPV6_V6ONLY),
    { NULL, 0.0 }
};
#undef X


static const duk_function_list_entry module_funcs[] = {
    /* name, function, nargs */
    { "socket", sock_socket, 2 },
    { "bind", sock_bind, 3 },
    { "connect", sock_connect, 3 },
    { "getsockname", sock_getsockname, 1 },
    { "getpeername", sock_getpeername, 1 },
    { "shutdown", sock_shutdown, 2 },
    { "listen", sock_listen, 2 },
    { "accept", sock_accept, 1 },
    { "recv", sock_recv, 2 },
    { "send", sock_send, 2 },
    { "recvfrom", sock_recvfrom, 2 },
    { "sendto", sock_sendto, 4 },
    { "inet_pton", sock_inet_pton, 2 },
    { "setsockopt", sock_setsockopt, 4 },
    { "getsockopt", sock_getsockopt, 4 },
    { "socketpair", sock_socketpair, 2 },
    { NULL, NULL, 0 }
};


DUK_EXTERNAL duk_ret_t sjs_mod_init(duk_context* ctx) {
    duk_push_object(ctx);
    duk_put_function_list(ctx, -1, module_funcs);
    duk_push_object(ctx);
    duk_put_number_list(ctx, -1, module_consts);
    duk_put_prop_string(ctx, -2, "c");
    return 1;
}

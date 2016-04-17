
#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

#include <sjs/sjs.h>


#define SJS__THROW_SOCKET_ERROR(error)                                                          \
    do {                                                                                        \
        duk_push_error_object(ctx, DUK_ERR_ERROR, "[errno %d] %s", (error), strerror((error))); \
        duk_push_int(ctx, (error));                                                             \
        duk_put_prop_string(ctx, -2, "errno");                                                  \
        duk_throw(ctx);                                                                         \
    } while (0)                                                                                 \


/*
 * Create a socket. Args:
 * - 0: domain
 * - 1: type
 * - 2: protocol
 */
static duk_ret_t sock_socket(duk_context* ctx) {
    int domain;
    int type;
    int protocol;
    int fd;

    domain = duk_require_int(ctx, 0);
    type = duk_require_int(ctx, 1);
    protocol = duk_require_int(ctx, 2);

    fd = socket(domain, type, protocol);
    if (fd == -1) {
        SJS__THROW_SOCKET_ERROR(errno);
        return -42;    /* control never returns here */
    } else {
        duk_push_int(ctx, fd);
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
            /* TODO: handle Linux abstract sockets */
            struct sockaddr_un* addru;
            const char* path;
            size_t path_len;
            addru = (struct sockaddr_un*) ss;
            duk_get_prop_string(ctx, addr_idx, "path");
            path = duk_require_lstring(ctx, -1, &path_len);
            strncpy(addru->sun_path, path, sizeof(addru->sun_path) - 1);
            addru->sun_path[sizeof(addru->sun_path) - 1] = '\0';
            addru->sun_family = AF_UNIX;
            *addrlen = sizeof(*addru);
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
static void sock__addr2obj(duk_context* ctx, const struct sockaddr* addr) {
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
            /* TODO: handle Linux abstract sockets */
            struct sockaddr_un* addru;
            size_t len;
            addru = (struct sockaddr_un*) addr;
            len = strlen(addru->sun_path);
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
        SJS__THROW_SOCKET_ERROR(errno);
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
        SJS__THROW_SOCKET_ERROR(errno);
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
        SJS__THROW_SOCKET_ERROR(errno);
        return -42;    /* control never returns here */
    }

    sock__addr2obj(ctx, (const struct sockaddr*) &ss);

    return 1;
}


static duk_ret_t sock_getsockname(duk_context* ctx) {
    return sock__getsockpeername(ctx, getsockname);
}


static duk_ret_t sock_getpeername(duk_context* ctx) {
    return sock__getsockpeername(ctx, getpeername);
}


/*
 * Close the socket. Args:
 * - 0: fd
 */
static duk_ret_t sock_close(duk_context* ctx) {
    int fd;

    fd = duk_require_int(ctx, 0);
    close(fd);

    duk_push_undefined(ctx);
    return 1;
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
        SJS__THROW_SOCKET_ERROR(errno);
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
        SJS__THROW_SOCKET_ERROR(errno);
        return -42;    /* control never returns here */
    }

    duk_push_undefined(ctx);
    return 1;
}


/*
 * Accept an incoming connection. Args:
 * - 0: fd
 */
static duk_ret_t sock_accept(duk_context* ctx) {
    int fd;
    int r;

    fd = duk_require_int(ctx, 0);

    r = accept(fd, NULL, NULL);
    if (r < 0) {
        SJS__THROW_SOCKET_ERROR(errno);
        return -42;    /* control never returns here */
    }

    duk_push_int(ctx, r);
    return 1;
}


/*
 * Read data from a socket. Args:
 * - 0: fd
 * - 1: nread
 * TODO:
 *  - use buffers
 *  - accept a list of buffers and use readv
 */
static duk_ret_t sock_recv(duk_context* ctx) {
    int fd;
    ssize_t r;
    size_t nread;
    char* buf;

    fd = duk_require_int(ctx, 0);
    nread = duk_require_int(ctx, 1);
    buf = malloc(nread);
    if (!buf) {
        SJS__THROW_SOCKET_ERROR(ENOMEM);
        return -42;    /* control never returns here */
    }

    r = recv(fd, buf, nread, 0);
    if (r < 0) {
        SJS__THROW_SOCKET_ERROR(errno);
        return -42;    /* control never returns here */
    } else if (r == 0) {
        /* EOF */
        duk_push_string(ctx, "");
    } else {
        duk_push_lstring(ctx, buf, r);
    }

    return 1;
}


/*
 * Write data to a socket. Args:
 * - 0: fd
 * - 1: data
 * TODO:
 *  - use buffers
 *  - accept a list of buffers and use readv
 */
static duk_ret_t sock_send(duk_context* ctx) {
    int fd;
    ssize_t r;
    size_t len;
    const char* buf;

    fd = duk_require_int(ctx, 0);
    buf = duk_require_lstring(ctx, 1, &len);

    r = send(fd, buf, len, 0);
    if (r < 0) {
        SJS__THROW_SOCKET_ERROR(errno);
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
 * - 1: nread
 * TODO:
 *  - use buffers
 */
static duk_ret_t sock_recvfrom(duk_context* ctx) {
    int fd;
    ssize_t r;
    size_t nread;
    char* buf;
    struct sockaddr_storage ss;
    socklen_t addrlen;

    addrlen = sizeof(ss);
    fd = duk_require_int(ctx, 0);
    nread = duk_require_int(ctx, 1);
    buf = malloc(nread);
    if (!buf) {
        SJS__THROW_SOCKET_ERROR(ENOMEM);
        return -42;    /* control never returns here */
    }

    r = recvfrom(fd, buf, nread, 0, (struct sockaddr*) &ss, &addrlen);
    if (r < 0) {
        SJS__THROW_SOCKET_ERROR(errno);
        return -42;    /* control never returns here */
    } else {
        duk_push_object(ctx);
        if (r == 0) {
            duk_push_string(ctx, "");
        } else {
            duk_push_lstring(ctx, buf, r);
        }
        duk_put_prop_string(ctx, -2, "data");
        if (addrlen > 0) {
            sock__addr2obj(ctx, (const struct sockaddr*) &ss);
        } else {
            duk_push_undefined(ctx);
        }
        duk_put_prop_string(ctx, -2, "address");
    }

    return 1;
}


/*
 * Send data to a unconnected socket. Args:
 * - 0: fd
 * - 1: data
 * - 2: address
 * TODO:
 *  - use buffers
 */
static duk_ret_t sock_sendto(duk_context* ctx) {
    int fd;
    ssize_t r;
    size_t len;
    const char* buf;
    struct sockaddr_storage ss;
    socklen_t addrlen;

    fd = duk_require_int(ctx, 0);
    buf = duk_require_lstring(ctx, 1, &len);

    if (sock__obj2addr(ctx, 2, &ss, &addrlen) != 0) {
        /* the stack top contains the error object */
        duk_throw(ctx);
        return -42;    /* control never returns here */
    }

    r = sendto(fd, buf, len, 0, (const struct sockaddr*) &ss, addrlen);
    if (r < 0) {
        SJS__THROW_SOCKET_ERROR(errno);
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


static const duk_number_list_entry module_consts[] = {
    /* socket domain */
    { "AF_INET", AF_INET },
    { "AF_INET6", AF_INET6 },
    { "AF_UNIX", AF_UNIX },
    /* socket type */
    { "SOCK_DGRAM", SOCK_DGRAM },
    { "SOCK_STREAM", SOCK_STREAM },
    /* shutdown how */
    { "SHUT_RD", SHUT_RD },
    { "SHUT_WR", SHUT_WR },
    { "SHUT_RDWR", SHUT_RDWR },
    { NULL, 0.0 }
};


static const duk_function_list_entry module_funcs[] = {
    /* name, function, nargs */
    { "socket", sock_socket, 3 },
    { "close", sock_close, 1 },
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

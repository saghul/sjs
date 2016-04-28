
#include <arpa/inet.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <sjs/sjs.h>


/* Parse the given sockaddr and leave an address object in the stack top. */
static void gai__parse_address(duk_context* ctx, struct sockaddr* addr) {
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
        default:
            abort();
    }
}


/*
 * Getaddrinfo. Args:
 * - 0: hostname
 * - 1: servname
 * - 2: hints
 */
static duk_ret_t gai_getaddrinfo(duk_context* ctx) {
    int r;
    const char* hostname;
    const char* servname;
    struct addrinfo hints;
    struct addrinfo* res;
    struct addrinfo* res0;

    hostname = duk_get_string(ctx, 0);
    servname = duk_get_string(ctx, 1);

    memset(&hints, 0, sizeof(hints));
    duk_get_prop_string(ctx, 2, "family");
    hints.ai_family = duk_get_int(ctx, -1);
    duk_pop(ctx);
    duk_get_prop_string(ctx, 2, "type");
    hints.ai_socktype = duk_get_int(ctx, -1);
    duk_pop(ctx);
    duk_get_prop_string(ctx, 2, "protocol");
    hints.ai_protocol = duk_get_int(ctx, -1);
    duk_pop(ctx);
    duk_get_prop_string(ctx, 2, "flags");
    hints.ai_flags = duk_get_int(ctx, -1);
    duk_pop(ctx);

    r = getaddrinfo(hostname, servname, &hints, &res0);
    if (r != 0) {
        duk_push_error_object(ctx, DUK_ERR_ERROR, "[gaierror %d] %s", r, gai_strerror(r));
        duk_push_int(ctx, r);
        duk_put_prop_string(ctx, -2, "code");
        if (r == EAI_SYSTEM) {
	    duk_push_int(ctx, errno);
	    duk_put_prop_string(ctx, -2, "errno");
	}
        duk_throw(ctx);
        return -42;    /* control never returns here */
    } else {
	int i = 0;
    	duk_push_array(ctx);
    	for (res = res0; res; res = res->ai_next) {
    	    if (res->ai_family != AF_INET && res->ai_family != AF_INET6) {
    	    	continue;
	    }

    	    /* object with family, socktype, protocol, canonname, and address */
    	    duk_push_object(ctx);

    	    duk_push_int(ctx, res->ai_family);
    	    duk_put_prop_string(ctx, -2, "family");
            duk_push_int(ctx, res->ai_socktype);
            duk_put_prop_string(ctx, -2, "type");
    	    duk_push_int(ctx, res->ai_protocol);
    	    duk_put_prop_string(ctx, -2, "protocol");
    	    duk_push_string(ctx, res->ai_canonname ? res->ai_canonname : "");
    	    duk_put_prop_string(ctx, -2, "canonname");
    	    gai__parse_address(ctx, res->ai_addr);
    	    duk_put_prop_string(ctx, -2, "address");
    	    /* stack: [... array obj ] */

    	    duk_put_prop_index(ctx, -2, i);
    	    i++;
	}
    }

    freeaddrinfo(res0);
    return 1;
}


static duk_ret_t gai_gstrerror(duk_context* ctx) {
    int r = duk_require_int(ctx, 0);
    const char* msg = gai_strerror(r);
    duk_push_string(ctx, msg);
    return 1;
}


#define X(name) {#name, name}
static const duk_number_list_entry module_consts[] = {
    /* ai_flags */
    X(AI_ADDRCONFIG),
    X(AI_ALL),
    X(AI_CANONNAME),
    X(AI_NUMERICHOST),
    X(AI_NUMERICSERV),
    X(AI_V4MAPPED),
    /* gai errors */
    X(EAI_AGAIN),
    X(EAI_BADFLAGS),
#ifdef EAI_BADHINTS
    X(EAI_BADHINTS),
#endif
    X(EAI_FAIL),
    X(EAI_FAMILY),
    X(EAI_MEMORY),
    X(EAI_NONAME),
    X(EAI_OVERFLOW),
#ifdef EAI_PROTOCOL
    X(EAI_PROTOCOL),
#endif
    X(EAI_SERVICE),
    X(EAI_SOCKTYPE),
    X(EAI_SYSTEM),
    { NULL, 0.0 }
};
#undef X


static const duk_function_list_entry module_funcs[] = {
    /* name, function, nargs */
    { "getaddrinfo", gai_getaddrinfo, 3 },
    { "gai_strerror", gai_gstrerror, 1 },
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

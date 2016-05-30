
.. _modnet:

net
===

This module provides access to networking utilities.


Socket object
-------------

The `Socket` object is a thin object oriented wrapper around :man:`socket(2)` and its related functionality.

.. js:class:: net.Socket(domain, type, [options])

    Creates a socket object.

    :param domain: Socket domain (one of :js:data:`net.AF_INET`, :js:data:`net.AF_INET6` or :js:data:`net.AF_UNIX`).
    :param type: Socket type (one of :js:data:`net.SOCK_STREAM` or :js:data:`net.SOCK_DGRAM`).
    :param options: Optional object which may contain the following properties:

        * `fd`: if specified, the socket is built with the given fd, an a new one is not created.
        * `nonBlocking`: if specified and ``true``, the socket will be breated in non-blocking mode.

    .. seealso::
        :man:`socket(2)`

    .. note::
        Since version 0.3.0 the fds are created with ``O_CLOEXEC`` set. You can undo this using :js:func:`os.cloexec`.

.. js:attribute:: net.Socket.fd

    Returns the file descriptor representing the socket.

.. js:function:: net.Socket.prototype.accept

    Waits for an incoming connection and accepts it. Only works on listening sockets.

    :returns: The accepted :js:class:`net.Socket` object.

    .. seealso::
        :man:`accept(2)`

.. js:function:: net.Socket.prototype.bind(address)

    Bind the socket to the given `address`. See :ref:`sockaddrs` for the format.

    .. seealso::
        :man:`bind(2)`

.. js:function:: net.Socket.prototype.close

    Closes the socket.

    .. seealso::
        :man:`close(2)`

.. js:function:: net.Socket.prototype.connect(address)

    Starts a connection towards the given `address`. See :ref:`sockaddrs` for the format.

    .. seealso::
        :man:`connect(2)`

.. js:function:: net.Socket.prototype.getsockname

    Returns the socket's local address. See :ref:`sockaddrs` for the format.

    .. seealso::
        :man:`getsockname(2)`

.. js:function:: net.Socket.prototype.getpeername

    Returns the socket's peer address. See :ref:`sockaddrs` for the format.

    .. seealso::
        :man:`getpeername(2)`

.. js:function:: net.Socket.prototype.listen([backlog])

    Set the socket in listening mode, ready for accepting connections.

    :param backlog: The maximum length for the queue of pending connections. If not set it defaults to 128.

    .. seealso::
        :man:`listen(2)`

.. js:function:: net.Socket.prototype.recv([nrecv])

    Receive data from a socket. It can only be used in a connected socket.

    :param nrecv: Maximum amount of data to receive. If not specified it defaults to 4096. Alternatively, a `Buffer`
        can be passed, and data will be read into it.
    :returns: The data that was read as a string or the amount of data read as a number, if a `Buffer` was passed.

    .. seealso::
        :man:`recv(2)`

.. js:function:: net.Socket.prototype.send(data)

    Transmit a message to the other socket. It can only be used with connected sockets.

    :param data: The message that will be transmitted (can be a string or a `Buffer`).
    :returns: The number of bytes from `data` which were actually sent.

    .. seealso::
        :man:`send(2)`

.. js:function:: net.Socket.prototype.recvfrom([nrecv])

    Similar to :js:func:`net.Socket.prototype.recv` but it can also be used in non-connected sockets.

    :returns: An object with 2 properties: `address`, which contains the address of the sender and `nread` if a
        `Buffer` was used, or `data`, with the data as a string.

    .. seealso::
        :man:`recvfrom(2)`

.. js:function:: net.Socket.prototype.sendto(data, address)

    Similar to :js:func:`net.Socket.prototype.recv` but it can also be used in non-connected sockets and the
    destination address has to be specified.

    .. seealso::
        :man:`sendto(2)`

.. js:function:: net.Socket.prototype.shutdown(how)

    Causes all or part of a full-duplex connection to be shut down. `how` must be one of :js:data:`net.SHUT_RD`,
    :js:data:`net.SHUT_WR` or :js:data:`net.SHUT_RDWR`.

    .. seealso::
        :man:`shutdown(2)`

.. js:function:: net.Socket.prototype.setsockopt(level, option, value)

    Set a socket option on the given `level`. `value` may contain either a number (for numeric or boolean  options) or
    a string containing the binary representation of the value. Use of `Buffer` objects is recommended to build the
    binary value.

    Example, setting a numeric or boolean option:

    ::

        var sock = new net.Socket(net.AF_INET, net.SOCK_STREAM);
        sock.setsockopt(net.SOL_SOCKET, net.SO_REUSEADDR, true);

    Example, setting a binary option:

    ::

        var sock = new net.Socket(net.AF_INET, net.SOCK_STREAM);
        var lingerOpts = new Buffer(8);
        lingerOpts.writeInt32LE(1, 0);    // enable lingering
        lingerOpts.writeInt32LE(100, 4);    // linger for 100 seconds
        sock.setsockopt(net.SOL_SOCKET, net.SO_LINGER, lingerOpts.toString());

    .. seealso::
        :man:`setsockopt(2)`

.. js:function:: net.Socket.prototype.getsockopt(level, option, [size])

    Get the value for a socket `option` on the given `level`. When `size` is omitted, the value is assumed to be
    an integer, but when a number is given a buffer of `size` size is used. A `Buffer` object can be used to parse
    the result.

    Example, getting a numeric or boolean option:

    ::

        var sock = new net.Socket(net.AF_INET, net.SOCK_STREAM);
        sock.setsockopt(net.SOL_SOCKET, net.SO_REUSEADDR, true);
        var r = sock.getsockopt(net.SOL_SOCKET, net.SO_REUSEADDR);

    Example, getting a binary option:

    ::

        var sock = new net.Socket(net.AF_INET, net.SOCK_STREAM);
        var lingerOpts = new Buffer(8);
        lingerOpts.writeInt32LE(1, 0);    // enable lingering
        lingerOpts.writeInt32LE(100, 4);    // linger for 100 seconds
        sock.setsockopt(net.SOL_SOCKET, net.SO_LINGER, lingerOpts.toString());
        var r = sock.sgetsockopt(net.SOL_SOCKET, net.SO_LINGER, 8);
        var resBuf = new Buffer(r);
        assert.equal(resBuf.readInt32LE(0), 1)
        assert.equal(resBuf.readInt32LE(4), 100)

    .. seealso::
        :man:`getsockopt(2)`

.. js:function:: net.Socket.prototype.setNonBlocking(set)

    Sets the socket in non-blocking mode if ``true``, or blocking mode if ``false``.


Constants
---------

.. js:data:: net.AF_INET

    IPv4 socket domain.

.. js:data:: net.AF_INET6

    IPv6 socket domain.

.. js:data:: net.AF_UNIX

    Unix socket domain.

.. js:data:: net.SOCK_STREAM

    Stream socket type.

.. js:data:: net.SOCK_DGRAM

    Datagram socket type.

.. js:data:: net.SHUT_RD
.. js:data:: net.SHUT_WR
.. js:data:: net.SHUT_RDWR

    Shutdown modes for :js:func:`net.Socket.prototype.shutdown`.


Functions
---------

.. js::function net.socketpair(domain, type, [options])

    Create a pair of conntected :js:class:`net.Socket` objects.

    :param domain: Socket domain (one of :js:data:`net.AF_INET`, :js:data:`net.AF_INET6` or :js:data:`net.AF_UNIX`).
        Typically only :js:data:`net.AF_UNIX` is supported.
    :param type: Socket type (one of :js:data:`net.SOCK_STREAM` or :js:data:`net.SOCK_DGRAM`).
    :param options: Optional object which may contain the following properties:

        * `nonBlocking`: if specified and ``true``, the sockets will be breated in non-blocking mode.

    .. seealso::
        :man:`socketpair(2)`

    .. note::
        Since version 0.3.0 the fds are created with ``O_CLOEXEC`` set. You can undo this using :js:func:`os.cloexec`.


.. _sockaddrs:

Socket addresses
----------------

Throughout this module, when an address is taken as a parameter or returned from a function, it's expressed as an
object with different properties, depending on the address family:

* IPv4 sockets (AF_INET family): object containing ``host`` and ``port`` properties.
* IPv6 sockets (AF_INET6 family): object containing ``host``, ``port``, ``flowinfo`` and ``scopeid`` properties. The
  last two can be omitted and will be assumed to be 0.
* Unix sockets (AF_UNIX family): string containing the path.


getaddrinfo
-----------

.. js:function:: net.getaddrinfo(hostname, servname, [hints])

    Get a list of IP addresses and port numbers for host `hostname` and service `servname`.
    See :man:`getaddrinfo(3)` for details.

    Example:

    ::

        sjs> var r = net.getaddrinfo('google.com', 'http');
        sjs> outil.inspect(r);
	= [ { family: 2,
	    type: 2,
	    protocol: 17,
	    canonname: '',
	    address: { host: '216.58.198.174', port: 80 } },
	{ family: 2,
	    type: 1,
	    protocol: 6,
	    canonname: '',
	    address: { host: '216.58.198.174', port: 80 } },
	{ family: 30,
	    type: 2,
	    protocol: 17,
	    canonname: '',
	    address:
	    { host: '2a00:1450:4009:809::200e',
	    port: 80,
	    flowinfo: 0,
	    scopeid: 0 } },
	{ family: 30,
	    type: 1,
	    protocol: 6,
	    canonname: '',
	    address:
	    { host: '2a00:1450:4009:809::200e',
	    port: 80,
	    flowinfo: 0,
	    scopeid: 0 } } ]

    The `hints` optional argument may contain an object with the following properties: ``family``, ``type``,
    ``protocol`` and ``flags``.

    Example:

    ::

        sjs> var r = net.getaddrinfo('google.com', 'http', {family: net.AF_INET});
	sjs> outil.inspect(r);
	= [ { family: 2,
	    type: 2,
	    protocol: 17,
	    canonname: '',
	    address: { host: '216.58.198.174', port: 80 } },
	{ family: 2,
	    type: 1,
	    protocol: 6,
	    canonname: '',
	    address: { host: '216.58.198.174', port: 80 } } ]

    The returned result is a list of objects containing the following properties: ``family``, ``type``, ``protocol``,
    ``canonname`` and ``address``.

.. js:function:: net.gai_strerror(code)

    Get the string that describes the given error `code`.

.. js:data:: net.gai_error_map

    A ``Map`` mapping `getaddrinfo` error codes to their string versions.

.. js:data:: net.AI_*

    All `addrinfo` constants to be used as hints in :js:func:`net.getaddrinfo`.
    See :man:`getaddrinfo(3)` for details.

.. js:data:: net.EAI_*

    All error codes :js:func:`net.getaddrinfo` could give.
    See :man:`getaddrinfo(3)` for details.

.. js:data:: net.SOL_*
.. js:data:: net.IPPROTO_*

    Levels to be used with :js:func:`net.Socket.prototype.setsockopt` and :js:func:`net.Socket.prototype.setsockopt`.

.. js:data:: net.SO_*
.. js:data:: net.IP_*
.. js:data:: net.IPV6_*
.. js:data:: net.TCP_*

    Options to be used with :js:func:`net.Socket.prototype.setsockopt` and :js:func:`net.Socket.prototype.setsockopt`.


Utility functions
-----------------

.. js:function:: net.isIP(address)

    Returns ``4`` if the given `address` is an IPv4 address, ``6`` if it's an IPv6 address and ``0`` otherwise.

.. js:function:: net.isIPv4(address)

    Returns ``true`` if the given `address` is a valid IPv4 address, and ``false`` otherwise.

.. js:function:: net.isIPv6(address)

    Returns ``true`` if the given `address` is a valid IPv6 address, and ``false`` otherwise.


# 0.2.0 (17-05-2016)

* d0636b9 vm: make module search function non-writable
* a112701 cli: exit CLI if Ctrl-C is pressed twice
* f6d5a9c build: don't rely on undocumented cmake options
* b7a3b39 build: add ability to set install prefix
* fedddc6 build: fix RPATH shenanigans (hopefully)
* 9e9d256 test: check that Duktape.modSearch is not writable
* 7bb69d5 time: fix sleeping from a fractional amnout of time
* fd24b85 vm: perform lazy binding when loading binary modules
* fd41c9e net: add support for Linux abstract sockets
* aadf3f3 time: fix computing time difference in hrtime
* aae4dbf test: adjust time test to be shorter
* 35710ea assert: fix composing stack trace
* fada5de vm: use the minified ES6 shim
* b640127 build: use -Wextra for debug builds
* 3e27550 build: fix compilation warnings
* e43c219 vm: simplify reading entire files
* 6b76fba build: log the build type
* cfe4369 ci: test debug and release builds on Travis
* a3bc5ab build: fix setting debug/release cflags
* d82eed1 build: link with librt on Linux
* 8a7a560 modules: add system.build
* 1584f39 doc: improve example for system.versions
* 85f6a93 misc: add AUTHORS
* a580db9 net: add support for sending and receiving buffers
* fb9fb84 cli: improve greet code
* 713d59e misc: fixup style
* 0eac06e net: add Socket.prototype.{get,set}sockopt
* 3045ba8 net: add net.socketpair
* 5a82a06 doc: update features
* 2a45335 misc: update README
* 91d0698 system: add system.endianness
* 0bae230 vm, cli: beautify command output
* c948253 modules: use exports instead of module.exports
* 72e968e cli: fix printing output if encoding fails
* ab56294 cli: strcasecmp function is defined in <strings.h>
* ed0db1f docu: document Socket.fd property
* 83bf599 modules: add fs module
* 420a93e core, modules: declare strict mode
* c28a9ad modules: add os module
* b22f9d0 test: se dynamic ports when testing with sockets
* b4cdc9d build: compile Duktape as a static library
* 0fbdae1 build: use -Werror in debug builds
* 1afdb6e build: silence compilation warning for Duktape
* 56d5a1b fs: fix compilation warning on Linux
* 3d98d90 os: add os.cwd
* 08a8dc2 os: add os.scandir
* 21bd8c9 build: use a single CMake file for all modules
* 218fd1a build: set common defines globally
* 7bebc68 ci: set umask to 022 on Travis CI
* c275e52 os: add os.stat
* 86fd87b build: document PREFIX and DESTDIR makefile variables
* 40b92f3 modules: remove fs module and merge it into io and os
* 191fef6 build: link libsjs required libraries, not sjs (cli)
* daa33d2 build: turn on -Wextra for all builds
* f9e53fe build: set version number in CMake file
* 2094703 ci: add GCC-5 to the build matrix
* 1dc1403 vm,cli: add ability to evaluate code in strict mode
* cd3c495 vm: normalize path before passing it to fopen
* 7ac4006 vm: expose a module-global __file__ attrbute
* 5f5c6dc vm: skip shebang when processing files
* 39d06d9 doc: document __file__
* 419354e doc: reorganize modules section
* 2372b25 core: set version to 0.2.0


# 0.1.0 (03-05-2016)

* Initial release

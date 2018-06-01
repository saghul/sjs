# 18.6.0 (01-06-2018)

* 98d5d3d modules: add hash module
* 08ed072 utils: fix typo
* 2cf72e4 modules: add console module
* 67ae323 vm: autoload module on bootstrap
* 632f09d doc: update FAQ
* 5eaeaca misc: update .gitignore
* 904003d vm: ignore errors when running bootstrap code
* 4b1edd0 vm: fix require.main
* 06a50f1 io: add File.writeLine
* 6ffdf5a io: simplified code
* 7fd8038 core: set name of CommonJS wrapper to "main"
* c3255b7 system: lazy load argv property
* c18fc48 system,io: move std{in,out,err} to system
* 13d72e4 system: add system.args
* 6f7a727 vm: update Duktape Node module extra
* 20432b6 core: use constant for maximum path length
* 3d4b0f9 misc: style
* 59c79ea doc: add asciinema demo
* 35e00c6 doc: fix sphinx warning
* 5fb3d3a modules: remove log module
* 9e03d41 console: wrap in a Proxy object
* a991ebd core: update Duktape to v2
* ae5fc76 cli: fix greeting message
* bd4acdd core: set version to 17.0.0 (alpha)
* 2805716 core: switch to babel-polyfill for modern JS shimming
* 0b8c672 ci: only test the master branch
* 805f4cf misc: updated copyright years
* b442884 core: update Duktape to v2.2.0
* 0e8fe41 vm: remove custom global object
* 75033aa time: remove no longer used time / hrtime
* be858bf ci: use macOS 10.12 + Xcode 9.1 for testing
* ce38fd9 modules: write error of module which couldn't be found
* ccfe420 test: only run tests if they end in .js
* d491a10 test: disable test/test-os-fork-execv
* 48a6547 vm: remove globals which don't really work (yet)
* ea574f4 core: refactor mechanism to bundle JS files
* f274c5d ci: update CI environments
* 4e763d6 test: fix test-os-stat
* eeee6d6 vm: use fread arguments consistently across the code
* cbff9ed os: add os.fstat
* ed32691 io: add io.readFile
* 9e00638 modules: remove unneeded code
* 7e271f5 core: bundle all native modules
* 03cad95 core: bundle all JavaScript code in libsjs
* d4d60a1 build: statically link libsjs into the sjs binary
* ab227da build: add shared library target
* 5e331db ci: update apt before installing packages
* 26463e8 core: update version
* 8ace450 cli: remove no longer needed import
* 6b1eb15 doc: updated Design section
* ccf3c5f bootstrap: add missing require
* 2b0a8dc cli: only show detailed build info on Debug builds
* 3bdb4f9 build: speedup build
* acd822c build: define DEBUG and NDEBUG depending on the build type
* 97219f7 cli: fix compilation warning
* 65acdfe io: fix compilation warning
* 50d4255 core: update duktape
* fe6c132 modules: fix compilation warning
* da3742e io: normalize the path before opening files
* bc35241 duktape: enable Object.prototype.__proto__
* ed42ae3 duktape: update build
* d2271c9 io,os,socket: return a binary buffer when reading data
* 73d7f3c test: port runner to Python 3 and run tests with a timeout
* 8196989 ci: install Python 3 in macOS
* ad3d4b7 doc: update documentation

# 0.4.0 (22-06-2016)

* 5dbd8d2 misc: updated changelog
* bcd7c1d ci: add musl libc to the test matrix
* 89b8b89 build: improve support for systems lacking execvpe
* 7832459 cli: group CLI related options in a global structure
* e43885e cli: add minimal line completion to the CLI
* 8cbd0b6 cli: simplified code
* ffef60d vm: add sjs_path_expanduser
* 20d7ea1 doc: man-up link
* afb6465 cli: make history file configurable
* f67a069 vm: fix adding extraneous / in sjs_path_expanduser
* b749995 doc: indicate the implementation used in Math.random
* 3cedadb cli: use sjs_vm_eval_code for running greeting code
* d34132e cli: fix running greeting code
* e3ad035 vm: simplified code
* 16ca862 test: simplify test
* 2569f12 core, vm: refactor module system
* 82d04e4 cli: fix running stdin code after executing a file or eval-ing code
* 66c8503 test: simplified test
* 5710d3e test: use new `__dirname` pseudo-global
* 2400909 modules: set require.main to the module when executing a file directly
* 16859d7 modules: use an anonymous function as the CommonJS wrapper
* dd8cf29 cli,vm: remove --use_strict flag
* 23c58a3 test: add test for require.main property
* 1c379f5 vm: fix require.main when eval-ing code from the CLI
* dbc6688 ci: test on Travis using GCC 6
* 9e8bf5e core: toggle some compile time Duktape options
* 528966d examples: add hello <name> example
* 67bc269 doc: fix highlightinh in index
* 7f72602 vm: don't add '.' and 'modules' to the default path
* 87b6cf0 test: add execve + read env in JSON test
* f066232 doc: add documentation about the module system
* 099da1d vm: add ability to get the VM reference from a Duktape context
* a4f1b3b codecs: add utf8 encoder / decoder
* bb82173 doc: move some definitions to the module system section
* 71f4b23 codecs: add punycode encoder / decoder
* 49c32a2 modules: move objectutils to utils/object
* a6e0871 assert: remove comment which no longer applies
* 11ba005 utils: add unicode submodule
* 60ea60f doc: improved utils/object docs
* d09deff vm: simplify pretty-printing objects


# 0.3.0 (06-06-2016)

* de023d7 vm: print error if opening file failed
* 2bb7372 os: add os.urandom
* 67b330d vm: don't support mod/index.jsdll construct
* bd4384a vm: fix require if directory and js file exist but dir is not usable
* a1eca61 objectutil: remove _extend
* e149381 net: remove unneeded value conversion
* 49e1a35 misc: simplified statements
* 3210a41 misc: use Object.assign to avoid modifying given parameters
* 0252f29 net: improved parameter validation
* e20a6af os: remove old comment
* 1cc8971 os: use arc4random_buf on OSX for os.urandom
* c08c965 build: add CFLAGS to CMAKE_C_FLAGS
* d889fe3 modules: add random module
* 916cb9c os: validate fd when opening /dev/urandom
* 5e77925 core: open all file descriptors with O_CLOEXEC
* 2d1612d os: fix spacing
* a261cb8 os: add os.fork, os.execve and related functions
* db28c76 net: use os.nonblock
* 0a75845 net: fix storing the non-blocking flag on the instance
* 78fedc1 doc: mention that O_CLOEXEC is now the default
* d912f97 os: add os.getpid and os.getppid
* d2e3d2a io: add missing io.fdopen
* d45e7ec vm: fix freeing memory on file read error
* ce710cb vm: error early if trying to read a directory
* ff53204 io: add readline function
* 7374519 os: add os.dup and os.dup2
* 489f2ff doc: fix typos
* 05cbc66 doc: add some section titles
* 009a4c2 os: add os.chdir
* 020f2c5 misc: add documentation and build status to README
* 01b392e io: fix typo
* b460709 os: refactor os.close
* dbf713b os: add os._exit
* b492288 os,system: move exit() to os module
* ebd634b modules: make system a non-builtin module
* a17b345 os: add os.setsid
* bb01c1d modules: add process module
* 7d12970 misc: new logo
* 015df52 random: remove unneeded cast
* 567f56c build,cli: fix compilation iwarning on ARM
* 1b75cb7 cli: update linenoise
* 201926f cli: use lineoise allocator function to free result
* e19211d cli: rename file
* 375f9a0 cli: remove duplicated includes
* a22fe9a test: add another execve test
* a7e8aef os: add os.execv
* d2176d2 doc: improve docs on os.execv/e
* 175dd08 io: add support for unbuffered stdio
* 3d8007c io: don't expose File objects, factory functions must be used
* 14e74a0 os: add execvp/e
* 1e949fe process: add process.spawn
* 4405aa8 os: add os.get{u,eu,g,eg}id
* 5a53aaf modules: add pwd module
* b1f5e08 os: add os.setuid and os.setgid
* 3ec5c09 os: add os.{get,set}groups
* e3d82e8 os: simplify exporting functions
* 83a2dd0 os: add STD{IN,OUT,ERR}_FILENO constants
* 02d6eb0 modules: add uuid module
* bc19d76 cli: use EXIT_SUCCESS and EXIT_FAILURE when exiting
* 1bc6cbe test: properly cleanup resources
* 8351eee doc: add process.spawn example
* 96dac7e doc: small fixups to README and documentation
* 482e2ff core: set version to 0.3.0


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

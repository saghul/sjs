
![sjs](https://raw.githubusercontent.com/saghul/sjs/master/docs/source/.static/sjs-logo.png)

# Overview

Skookum JS, or *sjs* for short, is a JavaScript runtime focused on providing
comprehensive POSIX APIs.

The motivation for this project comes from answering the question "how would a
JavaScript runtime look like if there were no browsers?".

# Features

* Small footprint
* [Ecmascript ES5/ES5.1](http://www.ecma-international.org/ecma-262/5.1/) compliant
* Some post-ES5 features
* Built-in Unicode support
* Tail call support
* CommonJS-based module loading system
* Support for native modules written in C
* Rich standard library
* Binary name 25% shorter than Node

See the full list [here](http://sjs.saghul.net/en/latest/features.html).

# Documentation

Check it out [here](http://sjs.saghul.net).

[![Documentation Status](https://readthedocs.org/projects/sjs/badge/?version=latest)](http://sjs.readthedocs.io/en/latest/?badge=latest)

# Building

Skookum JS currently supports Linux and macOS. See compilation instructions [here](http://sjs.saghul.net/en/latest/#building).

[![Build Status](https://travis-ci.org/saghul/sjs.svg?branch=master)](https://travis-ci.org/saghul/sjs)

# License

Check [the LICENSE file](https://github.com/saghul/sjs/blob/master/LICENSE).

# Author

Saúl Ibarra Corretgé ([**@saghul**](https://github.com/saghul))

# Contributing

Please see [CONTRIBUTING](https://github.com/saghul/sjs/blob/master/CONTRIBUTING.md).

# Acknowledgements

sjs would not have been possible without some other projects existing. Some served
as a source of inspiration, some as a source of actual code, and the author would like to thank them all.

* [Duktape](http://duktape.org/): The engine that powers sjs. This project would not have been possible without Duktape. :hearteyes:
* [Nodejs](https://nodejs.org): World's most well known JavaScript runtime, definitely an inspiration.
* [libuv](https://github.com/libuv/libuv): A lot of the cross-platform code was borrowed from here.
* [Dukluv](https://github.com/creationix/dukluv): Another JavaScript runtime combining Duktape with libuv.
* [CPython](https://github.com/python/cpython/): Python's canonical interpreter was an inspiration, specially for the standard library components.

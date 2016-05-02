
# Skookum JS

Skookum JS, or *sjs* for short, is a JavaScript runtime focused on giving
unobtrusive and straight abstractions to low level platform facilities.

The motivation for this project comes from answering the question "how would a
JavaScript runtime look like if there were no browsers?".

## Hello, world

    Skookum JS 0.0.1dev on osx (x64)
    [Duktape v1.4.0-356-gf1e68db (f1e68db)]
    sjs> print('hello world!');
    hello world!
    = undefined

## Documentation

Check it out [here](TODO).

## Building

Skookum JS currently supports Linux and OSX. See compilation instructions [here](TODO).

## License

Check [the LICENSE file](TODO). Hint: it's MIT.

## Author

Saúl Ibarra Corretgé ([**@saghul**](https://github.com/saghul))

## Contributing

Please see [CONTRIBUTING](TODO).

## Acknowledgements

sjs would not have been possible without some other projects existing. Some served
as a source of inspiration, some as a source of actual code, and the author would like to thank them all.

* [Duktape](http://duktape.org/): The engine that powers sjs. This project would not have been possible without Duktape. :hearteyes:
* [Nodejs](https://nodejs.org): World's most well known JavaScript runtime, definitely an inspiration.
* [libuv](https://github.com/libuv/libuv): A lot of the cross-platform code was borrowed from here.
* [Dukluv](https://github.com/creationix/dukluv): Another JavaScript runtime combining Duktape with libuv.
* [CPython](https://github.com/python/cpython/): Python's canonical interpreter was an inspiration, specially for the standard library components.

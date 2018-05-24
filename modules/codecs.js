'use strict';

const punycode = require('_codecs_punycode');
const utf8     = require('_codecs_utf8');


var encoders = {
    'base64'  : Duktape.enc.bind(Duktape, 'base64'),
    'hex'     : Duktape.enc.bind(Duktape, 'hex'),
    'json'    : JSON.stringify,
    'punycode': punycode.encode,
    'utf8'    : utf8.encode
};


var decoders = {
    'base64'  : Duktape.dec.bind(Duktape, 'base64'),
    'hex'     : Duktape.dec.bind(Duktape, 'hex'),
    'json'    : JSON.parse,
    'punycode': punycode.decode,
    'utf8'    : utf8.decode
};


function encode(encoding, data) {
    const encoder = encoders[encoding];
    if (!encoder) {
        throw new Error('encoder not found: ' + encoding);
    }
    return encoder(data);
}


function decode(encoding, data) {
    const decoder = decoders[encoding];
    if (!decoder) {
        throw new Error('decoder not found: ' + encoding);
    }
    return decoder(data);
}


exports.encode = encode;
exports.decode= decode;

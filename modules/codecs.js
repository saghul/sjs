'use strict';

var encoders = {
    'base64': Duktape.enc.bind(global, 'base64'),
    'hex': Duktape.enc.bind(global, 'hex'),
    'json': JSON.stringify
};


var decoders {
    'base64': Duktape.dec.bind(global, 'base64'),
    'hex': Duktape.dec.bind(global, 'hex'),
    'json': JSON.parse
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


module.exports.encode = encode;
module.exports.decode = decode;

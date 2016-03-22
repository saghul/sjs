'use strict';

function encode(data) {
    return Duktape.enc('base64', data);
}


function decode(data) {
    return Duktape.dec('base64', data);
}


module.exports.encode = encode;
module.exports.decode = decode;

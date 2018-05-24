'use strict';

const unorm = require('_utils_unicode_unorm');


function normalize(form, str) {
    form =  form === undefined ? "NFC" : form;

    if (form === "NFC") {
        return unorm.nfc(str);
    } else if (form === "NFD") {
        return unorm.nfd(str);
    } else if (form === "NFKC") {
        return unorm.nfkc(str);
    } else if (form === "NFKD") {
        return unorm.nfkd(str);
    } else {
        throw new RangeError("Invalid normalization form: " + form);
    }
}


exports.normalize = normalize;

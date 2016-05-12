'use strict';

const assert = require('assert');

function replaceModSearch() {
    Duktape.modSearch = function() {};
}

assert.throws(replaceModSearch, TypeError);

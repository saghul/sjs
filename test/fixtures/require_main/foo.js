'use strict';

const assert = require('assert');
const bar = require('./bar');


assert(require.main !== undefined);
assert(require.main === module);

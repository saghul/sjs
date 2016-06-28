'use strict';

const codecs = require('codecs');
const sys = require('system');


sys.stdout.write(JSON.stringify(sys.env));
sys.stdout.flush();

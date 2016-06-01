'use strict';

const os = require('os');
const system = require('system');


var fd = Number.parseInt(system.env.PARENT_FD);
os.write(fd, system.argv.slice(2).join(','));


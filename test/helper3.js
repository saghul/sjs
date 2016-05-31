'use strict';

const io = require('io');
const os = require('os');


var data = io.stdin.readline();
io.stdout.write(data);
io.stdout.flush();
io.stderr.write(data);
io.stderr.flush();

os.exit(0);

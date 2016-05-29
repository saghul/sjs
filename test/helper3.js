'use strict';

const io = require('io');


var data = io.stdin.readline();
io.stdout.write(data);
io.stdout.flush();
io.stderr.write(data);
io.stderr.flush();

system.exit(0);

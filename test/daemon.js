'use strict';

const time = require('time');
const process = require('process');


// go into the background
process.daemonize();

// wait forever
for (;;) {
    time.sleep(1);
}

'use strict';

const os = require('os');


// usual double forking for Unix daemons

function daemonize() {
    var pid1 = os.fork();
    if (pid1 != 0) {
        // exit parent
        os._exit(0);
    }

    // become a session leader
    os.setsid();

    // change working directory
    os.chdir('/');

    // replace stdio
    var devNull = os.open('/dev/null', os.O_RDWR);
    os.dup2(devNull, 0, false);
    os.dup2(devNull, 1, false);
    os.dup2(devNull, 2, false);
    if (devNull > 2) {
        os.close(devNull);
    }

    var pid2 = os.fork();
    if (pid2 != 0) {
        // exit parent
        os._exit(0);
    }
}


exports.daemonize = daemonize;

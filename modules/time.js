'use strict';

const _time = require('time');
const select = require('io/select');


function sleep(delay) {
    select.select([], [], [], delay >>> 0);
    // TODO: handle EINTR? return remaining time?
}


module.exports = {
    time   : _time.time,
    hrtime : _time.hrtime,
    sleep  : sleep
};

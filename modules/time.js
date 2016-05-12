'use strict';

const _time = require('_time');
const select = require('io/select');


function sleep(delay) {
    select.select([], [], [], delay);
    // TODO: handle EINTR? return remaining time?
}


exports.time   = _time.time;
exports.hrtime = _time.hrtime;
exports.sleep  = sleep;

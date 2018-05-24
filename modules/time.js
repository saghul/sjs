'use strict';

const select = require('io').select;


function sleep(delay) {
    select.select([], [], [], delay);
    // TODO: handle EINTR? return remaining time?
}


exports.sleep  = sleep;

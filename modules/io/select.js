'use strict';

const _select = require('_select');


function select(rfds, wfds, xfds, timeout) {
    if (timeout == null) {
        timeout = null;
    } else {
        timeout = parseFloat(timeout);
        if (isNaN(timeout) || timeout < 0) {
            throw new TypeError('timeout must be null or a positive number')
        }
    }
    // fds are sanitized inside the C module
    return _select.select(rfds || [], wfds || [], xfds || [], timeout);
}


exports.select     = select;
exports.FD_SETSIZE = _select.c.FD_SETSIZE;

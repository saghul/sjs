
const fs = require('fs');
const _fs = require('_fs');


const stdin = new fs.File('<stdin>', _fs.stdin(), 'r');
const stdout = new fs.File('<stdout>', _fs.stdout(), 'w');
const stderr = new fs.File('<stderr>', _fs.stderr(), 'w');


exports.stdin  = stdin;
exports.stdout = stdout;
exports.stderr = stderr;

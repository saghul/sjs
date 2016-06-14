'use strict';

const io = require('io');


io.stdout.write('What is your name? ');
var name = io.stdin.readline();
io.stdout.write('Hello ' + name.trim() + ', nice to meet you!\n');

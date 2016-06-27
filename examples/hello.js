'use strict';

const io = require('io');


io.stdout.write('What is your name? ');
var name = io.stdin.readLine();
io.stdout.writeLine('Hello ' + name.trim() + ', nice to meet you!');

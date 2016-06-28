'use strict';

const sys = require('system');


sys.stdout.write('What is your name? ');
var name = sys.stdin.readLine();
sys.stdout.writeLine('Hello ' + name.trim() + ', nice to meet you!');

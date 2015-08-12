var path = require('path');
var fs   = require('fs');
var util = require('util');



var raw = '@jamiezigelbaum #avgECBAA soup nazi seinfeld';
var strip = raw.replace(/([@])\w+\s/g, '');
var strip1 = strip.replace(/([#])\w+\s/g, '');
var command = '#avgECBAA';
console.log(strip1);
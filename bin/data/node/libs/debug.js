/* -*- C++ -*- */

/**
 * simple node.js debugging.
 */

var util     = require('util');
var readline = require('readline');
var debug    = exports;

if (typeof DEBUGGING == 'undefined') {
  DEBUGGING = false;
}

debug.log = util.log;
debug.log('NodeJS started with PID: ' + process.pid);
debug.dir = function (data) {
  debug.log(util.inspect(data, {showHidden: true, depth: null, colors: true}));
}

var microtime = function (get_as_float)
{
  var now = new Date().getTime() / 1000;
  var s = parseInt(now, 10);
  return (get_as_float) ? now : (Math.round((now - s) * 1000) / 1000) + ' ' + s;
}


/**
 * Turn on memory usage and uptime tracking if we're debugging.
 */

if (DEBUGGING == true) {
  start_time   = (new Date()).getTime();
  memory_usage = process.memoryUsage();

  setInterval(function () {
      memory_usage = process.memoryUsage();
    }, 5000);
}



debug.log('Loaded: ' + __filename);

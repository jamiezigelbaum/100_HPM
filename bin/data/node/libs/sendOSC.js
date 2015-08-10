var osc 		= require('node-osc');
var client 		= new osc.Client('127.0.0.1', 7778);
var escaped_str = require('querystring');
var queue         = require('./queue');
var queue         = new queue();
var sendOSC     = exports;
sendOSC.timer   = null;
sendOSC.config  = {"interval" : 500};

sendOSC.start = function ()
{
  sendOSC.timer = setInterval(sendOSC.process_job, sendOSC.config.interval);
};

sendOSC.stop = function ()
{
  if (sendOSC.timer) {
    clearTimeout(sendOSC.timer);
    sendOSC.timer = null;
  }
};

sendOSC.sendPacket = function (path, file)  
{
	client.send(path, file);
};

sendOSC.process_job = function ()
{
  var job = queue.get_job();
  if (job == false) {
    return false;
  }

  var address = job.get_data().path;
  var data = job.get_data().data;
  sendOSC.sendPacket(address, data);
};

sendOSC.send = function (address, data)
{
	queue.add('get_data', { "path": address, "data" :data }, 'add osc data');
};




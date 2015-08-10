/* -*- C++ -*- */

/**
 * POC queue system
 *
 * This really should not be used for anything important. It was only written
 * because most of the pre-built queue systems require redis or some other backend.
 */


var debug  = require('./debug');
var events = require('events');


/**
 * Job description
 */

var job    = function (job_name, job_data, job_description)
{
  events.EventEmitter.call(this);

  this.test = 'wtf';

  this.get_data = function()
  {
    return job_data;
  }

  this.completed = function()
  {
    this.emit('complete');
  }

  this.failed = function()
  {
    this.emit('failed');
  }
}

job.prototype.__proto__ = events.EventEmitter.prototype;


/**
 * Queue class.
 */

var queue = function ()
{
  this.jobs = [];

  /**
   * Add a job to the queue
   */
   this.clear = function(){
    this.jobs = [];
   }

  this.add = function (job_name, job_data, job_description)
  {
    var task = new job (job_name, job_data, job_description);

    task.on('complete', function(){ debug.log('Job ' + job_description + ' complete.'); });
    task.on('failed',   function(){ debug.log('Job ' + job_description + ' FAILED!'); });

    this.jobs.unshift(task);
  }

  /**
   * Take a job from the queue
   */

  this.get_job = function ()
  {
    if (this.jobs.length == 0) {
      return false;
    }

    return this.jobs.pop();
  }
};




module.exports = queue;




debug.log('Loaded: ' + __filename);


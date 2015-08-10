var _ = require('underscore');
var fs = require('fs');
var mime = require('mime');
var util = require('util');
var config = require('../../config/googleyoutube.json');
var youtube = require('./googleyoutube');
var youtube = new youtube();
var osc = require('node-osc');
var queue = require('../queue');
var queue  = new queue();
var twitter = require('./twitter');
var upload = exports;

upload.start = function(){
    upload.timer = setInterval(upload.process_job, 2000);

	var oscServer = new osc.Server(7779, '127.0.0.1');
	oscServer.on("message", function (msg, rinfo) {
		console.log(msg);
			var data = {'user':msg[2][1], 'query':msg[2][2], 'renderPath':msg[2][3]};
			console.log(JSON.stringify(data));
			queue.add('get_data', {'video': data }, 'add upload');
	});
}


upload.process_job = function ()
{
  var job = queue.get_job();
  if (job == false) {
    return false;
  }
  var data = job.get_data().video;
  file = config.video_dir_path+data.renderPath;
	var mimeType = mime.lookup(file);
	if(mimeType.indexOf("video/") === 0){
		var options = {
			"title":util.format("%s search by %s via 100 Hours per Minute", data.query, data.user),
			"description": config.description,
			"privacyStatus": "public",
			"videofile": file
		};
		youtube.uploadVideo(options,function(err,res){
			if(err){
				console.log('Error while uploading video',err);
			}else{
				console.log('\n Video has been successfully uploaded \n',res);
				console.log('\n Check your uploaded video using:\n https://www.youtube.com/watch?v='+res.id);
				var postMessage = {'user':data.user, 'url':'http://www.youtube.com/watch?v='+res.id, 'query':data.query}
				twitter.postTweet(postMessage);
			}
		});
	}
}
	

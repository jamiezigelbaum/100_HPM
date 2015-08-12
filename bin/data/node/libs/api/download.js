
var path = require('path');
var fs   = require('fs');
var util = require('util')
var ytdl = require('ytdl-core');
var queue = require('../queue');
var osc = require('../sendOSC');
var queue  = new queue();
var done = true;
var send = false;
var download  = exports;
var currentSearch = ""
var count;
download.timer = null;

download.start = function(){
	download.timer = setInterval(download.process_job, 100);
	done = true;
	osc.start();
	count = 0;
}

download.add = function(videoid, search){
	console.log(search+" "+videoid);
	queue.add('get_data', {'video_id':videoid, 'search':search}, 'add video to download queue');
}


download.process_job = function ()
{

	if(!done || count > 2){
		return false;
	}
  	var job = queue.get_job();

	if(job == false){
		return false;
	}
	var item = job.get_data().video_id;
	var search = job.get_data().search;


	if (!fs.existsSync(path.join(__dirname+'/../../../../data/movies/', item+".mp4"))) {

		var foo  = ytdl(util.format('http://www.youtube.com/watch?v=%s', item), { filter: function(format) { return format.container === 'mp4'; }, format:'18'});
		if(foo){
			var picStream  = fs.createWriteStream(path.join(__dirname+'/../../../../data/movies/', item+".mp4")); 
			picStream.on('open', function() {
					console.log(util.format('download started %s', item));
		   			done = false;
			});
			picStream.on('close', function() {
					console.log(util.format('download finished %s', item));
					var videoData = {'video':item+".mp4", 'search':search}
		  			osc.send("/newvideo", JSON.stringify(videoData));
		  			done = true;
		  			count--;
			});


			foo.on('error', function(err) {
	  			console.error(err.message);
	  			done = true;
	  			picStream.close();
	  			fs.unlink(path.join(__dirname+'/../../../../data/movies/', item+".mp4"), function (err) {
				  if (err)
				  console.log(util.format('deleted error %s.mp4', item));
                  var videoData = {'video':item+".mp4", 'search':search}
                  osc.send("/errror", JSON.stringify(videoData));
				});
                   
			});
		
			foo.pipe(picStream);
		}
	}else{
		console.log(util.format('file exists %s.mp4', item));
		var videoData = {'video':item+".mp4", 'search':search}
		osc.send("/newvideo", JSON.stringify(videoData));
		done = true;
	}
}

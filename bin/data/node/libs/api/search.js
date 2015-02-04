var path = require('path');
var fs   = require('fs');
var util = require('util');
var youtubeQueue = require('./download');
var YouTube = require('youtube-node');
var osc = require('../sendOSC');
var queue = require('../queue');
var queue  = new queue();
var youTube = new YouTube();
var search  = exports;
var currentHash = '#0000';
var randomstring = require("randomstring");
search.time = null;

youtubeQueue.start();

youTube.setKey('AIzaSyB4X5DCsHWMfvTNM-hSrSa8c3yNyVAt4vA');

search.start = function(){
    search.timer = setInterval(search.process_job, 500);
    osc.start();
    currentHash = util.format("#%s",randomstring.generate(4)).toLowerCase();
    console.log(currentHash);
    osc.send('/newHash', currentHash);
}

search.sendErrorHash = function(){
    currentHash = "#ConnectionError";
     osc.send('/newHash', "#ConnectionError");
}

search.add = function (data)
{
  var raw = data['tweet_text'];
  if(raw.indexOf(currentHash) > -1){
    var strip = raw.replace(/([@])\w+\b/g, '');
    var strip1 = strip.replace(/([#])\w+\b/g, '');
    var username = data['user_screen_name'];
    var searchData = {'search':strip1, 'username': username, 'videos':[]};
    queue.add('get_data', {'query': searchData }, 'add youtube search');
    currentHash = util.format("#%s",randomstring.generate(4)).toLowerCase();
    console.log(currentHash);
    osc.send('/newHash', currentHash);
  }
};

search.testadd = function (data)
{
    var raw = data['tweet_text'];
    var strip = raw.replace(/([@])\w+\s/g, '');
    var strip1 = strip.replace(/([#])\w+\s/g, '');
    var username = data['user_screen_name'];
    var searchData = {'search':strip1, 'username': username, 'videos':[]};
    queue.add('get_data', {'query': searchData }, 'add youtube search');
    currentHash = util.format("#%s",randomstring.generate(4)).toLowerCase();
    console.log(currentHash);
    osc.send('/newHash', currentHash);
};

search.process_job = function ()
{
  osc.send('/newHash', currentHash);
  var job = queue.get_job();
  if (job == false) {
    return false;
  }
  var data = job.get_data().query;
  var query = job.get_data().query['search'];
  youTube.search(query, 30, function(resultData) {
      console.log(JSON.stringify(resultData));
       for(var item in resultData["items"]){  
        if(resultData['items'][item]['id']['videoId']){
          console.log(resultData['items'][item]['id']['videoId']);
          data['videos'].push({'id':resultData['items'][item]['id']['videoId']});
          youtubeQueue.add(resultData['items'][item]['id']['videoId'], query);
        }
       }
       // console.log(JSON.stringify(encodeURI(data));
       osc.send('/data', JSON.stringify(data));
  });
}

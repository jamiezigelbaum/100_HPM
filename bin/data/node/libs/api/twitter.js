/* -*- C++ -*- */

/**
 * POC Twitter Stream Scraper
 *
 */
var util          = require('util');
var debug         = require('../debug');
var sha1          = require('../sha1');
var ntwitter      = require('ntwitter');
var post_json     = require('../post_json');
var query        = require('../../config/twitter.json');
var strCopy        = require('../../config/text.json');
var search        = require('./search');
var emojiStrip    = require('emoji-strip');
var stripCombiningMarks = require('strip-combining-marks');
var twitter     = exports;
debug.log('Loaded: ' + __filename);

twitter.ntwitter = new ntwitter({
                                "consumer_key"        : "qVIBXjIISwyXDBNdGfMvSEJxd",
                                "consumer_secret"     : "ZXYR9xGB9M7NYBXDLKyetd5SUIIq1x2n3O4HoOXpDoD9dLXU3B",
                                "access_token_key"    : "2980194508-bfE2X6MryXU1Q72avToCygmRoLDOBFMtn5YuYgq",
                                "access_token_secret" : "x5KSnoXOSfHVFOH2zX229w471WprfWVCKI2UOPE8CjhFD"
                                });


twitter.start = function(data)
{
    
    debug.log('Connecting to twitter stream...');
    
    twitter.ntwitter.stream('statuses/filter', query, function (stream) {
                            
                            twitter.stream = stream;
                            
                            twitter.stream.on('data', twitter.parse);
                            
                            twitter.stream.on('error', function (data, more_data, some_more) {
                                              debug.log('TWITTER STREAM ERROR!');
                                              debug.dir(data);
                                              setTimeout(twitter.start, 14000);
                                              search.sendErrorHash();
                                              });
                            
                            twitter.stream.on('destroy', function (response) {});
                            });
    search.start();
    
    if(data == true){
        search.testadd({'user_screen_name':'thedantheman','tweet_text':'@thedantheman digital art'});
        search.testadd({'user_screen_name':'thedantheman','tweet_text':'@thedantheman video art'});
        search.testadd({'user_screen_name':'thedantheman','tweet_text':'@thedantheman light art'});
        search.testadd({'user_screen_name':'thedantheman','tweet_text':'@thedantheman performance art'});
        search.testadd({'user_screen_name':'thedantheman','tweet_text':'@thedantheman fine art'});
    }
};

twitter.start_user_stream = function()
{
    debug.log('Connecting to twitter user stream...');
    
    twitter.ntwitter.stream('user', {}, function (user_stream) {
                            
                            twitter.user_stream = user_stream;
                            
                            twitter.user_stream.on('data', twitter.parse);
                            
                            twitter.user_stream.on('error', function (data, more_data, some_more) {
                                                   debug.log('TWITTER USER STREAM ERROR!');
                                                   debug.dir(data);
                                                   
                                                   setTimeout(twitter.start_user_stream, 60000);
                                                   });
                            
                            twitter.user_stream.on('destroy', function (response) {});
                            });
};


twitter.postTweet = function(tweeet){
    console.log(tweeet);
    twitter.ntwitter.verifyCredentials(function (err, data) {console.log(data);})
    .updateStatus(util.format(strCopy.tweet, tweeet.user, tweeet.query, tweeet.url), function (err, data) {console.log(data);});
}
/**
 * Parse the tweet to elasticsearch or whatever.
 */

twitter.parse = function (data)
{
    if (!data.id) {
        debug.log("Broken tweet received.");
        debug.dir(data);
        return false;
    }
    
    var full_tweet = { "GUID"             : sha1('twitter' + data.id),
        "created_at"       : data.created_at,
        "tweet_text"       : data.text,
        "id"               : data.id,
        "user_id"          : data.user.id,
        "user_name"        : data.user.name,
        "user_screen_name" : data.user.screen_name,
        "longitude"        : ((!data.geo || !data.geo.coordinates) ? null : data.geo.coordinates[0]),
        "latitude"         : ((!data.geo || !data.geo.coordinates) ? null : data.geo.coordinates[1]),
        "place_id"         : ((!data.place || !data.place.id) ? null : data.place.id),
        "place_url"        : ((!data.place || !data.place.url) ? null : data.place.url),
        "place_name"       : ((!data.place || !data.place.full_name) ? null : data.place.full_name),
        "retweet_count"    : data.retweet_count,
        "favorite_count"   : data.favorite_count,
        "was_retweet"      : data.retweeted,
        "was_favorite"     : data.favorited,
        "user_image"       : data.user.profile_image_url
    };
    
    
    var tweet_date = new Date(data.created_at).toISOString().toString();
    var guid       = sha1('twitter' + data.id);
    
    var social_feed_item = {"id"                 : full_tweet.id,
        "user_name"          : stripCombiningMarks(emojiStrip(full_tweet.user_name)),
        "user_screen_name"   : stripCombiningMarks(emojiStrip(full_tweet.user_screen_name)),
        "tweet_text"         : stripCombiningMarks(emojiStrip(full_tweet.tweet_text))
    };
    
    //debug.log('ADD [TWEET] ' + data.id);
    
    twitter.add(social_feed_item);
};


/**
 * Add the tweet to elasticsearch or whatever.
 */

twitter.add = function (tweet)
{
    search.add(tweet);
};

twitter.stop = function()
{
    if (twitter.stream && twitter.stream.destroy) {
        twitter.stream.destroy();
    }
};

twitter.stop_user_stream = function()
{
    if (twitter.user_stream && twitter.user_stream.destroy) {
        twitter.user_stream.destroy();
    }
};


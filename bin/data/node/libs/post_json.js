/* -*- C++ -*- */

/**
 * Simple functionality to post JSON to a remote URL
 *
 */

var debug       = require('./debug');
var http        = require('http');
var querystring = require('querystring');
var https       = require('https');

module.exports = function (http_options, post_data, callback, params) {

  try {

    http_options.headers['Content-length'] = post_data.length;

    http_request = http.request(http_options, function (httpClientResponse) {

        var bodyResponse = '';

        //debug.log('STATUS: ' + httpClientResponse.statusCode);
        //debug.log('HEADERS: ' + JSON.stringify(httpClientResponse.headers));

        httpClientResponse.setEncoding('utf8');
        httpClientResponse.on('data', function (chunk) {
            callback(chunk, params);
          });
      });

    debug.log('POSTING to: ' + http_options.hostname + http_options.path);

    //debug.dir(post_data);

    http_request.write(post_data);
    http_request.end();

  }
  catch (error) {
    debug.log('ERROR - Network error when trying to post data.');
    debug.dir(error);
    debug.log('--------- DATA ---------');
    debug.dir(post_data);
    debug.log('--------- DATA ---------');
  }
};


debug.log('Loaded: ' + __filename);

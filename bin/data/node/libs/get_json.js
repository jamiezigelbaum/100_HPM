/* -*- C++ -*- */

/**
 * Simple functionality to grab JSON from a remote URL
 *
 */

var debug      = require('./debug');
var http       = require('http');
var https      = require('https');

module.exports = function (url, callback, params) {

  var http_method = http;

  if (url.substring(0,5) == 'https') {
    http_method = https;
  }

  try {

    http_method.get(url, function (httpClientResponse) {

        var bodyResponse = '';

        //debug.log('Trying to get JSON from URL: ' + url);
        debug.log('GET ' + url);

        httpClientResponse.on('data', function (data_chunk) {
            bodyResponse += data_chunk;
        });

        httpClientResponse.on('end', function() {

            debug.log('GOT ' + url);

            var bodyJSON = {};

            try {
              bodyJSON = JSON.parse(bodyResponse);
            }
            catch (error) {
              debug.log('ERROR PARSING JSON:');
              debug.log(bodyResponse);
            }

            //debug.log('Received JSON for URL: ' + url);
            //debug.log(JSON.stringify(bodyJSON));
            callback(bodyJSON, params);
          });

      }).on('error', function (error) {
          debug.log('ERROR - HTTP error when trying to query URL: ' + url);
          debug.dir(error);
        })
  }
  catch (error) {
    debug.log('ERROR - Network error when trying to query URL: ' + url);
    debug.dir(error);
  }
};


debug.log('Loaded: ' + __filename);

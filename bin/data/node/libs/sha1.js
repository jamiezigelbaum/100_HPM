/* -*- C++ -*- */

/**
 * Wrapper function to make SHA1 hashes.
 */

var debug      = require('./debug');
var crypto     = require('crypto');

module.exports = function (string) {
  var sha_sum    = crypto.createHash('sha1');
  sha_sum.update(string);

  return sha_sum.digest('hex');
};

debug.log('Loaded: ' + __filename);

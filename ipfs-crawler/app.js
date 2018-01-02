var ipfsAPI = require('ipfs-api');
var ipfs = ipfsAPI({host: 'localhost', port: '5001', protocol: 'http'});

/*
ipfs.util.addFromFs('/tmp/test.txt',{ recursive: false},function(err, result) {
  if (err) {
    throw err;
  }
  console.log('result=<',result,'>');
});
*/

var client = redis.createClient();
var redis = require('redis');
console.log('redis=<',redis,'>');

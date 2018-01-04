var redis = require('redis');
var ipfsAPI = require('ipfs-api');

var ipfs = ipfsAPI({host: 'localhost', port: '5001', protocol: 'http'});
var refs = ipfs.refs.local((err,result)=>{
  console.log('err=<',err,'>');
  console.log('result=<',result,'>');
});
console.log('refs=<',refs,'>');


//var client = redis.createClient();
  

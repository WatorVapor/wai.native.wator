var redis = require('redis');
var ipfsAPI = require('ipfs-api');

var ipfs = ipfsAPI({host: 'localhost', port: '5001', protocol: 'http'});

/*
var refs = ipfs.refs.local((err,result)=>{
  console.log('err=<',err,'>');
  console.log('result=<',result,'>');
});
console.log('refs=<',refs,'>');
*/


const redisKeyPrefix = '/wator/wai/crawler/wiki';
const redisKeyPrefixDone = '/wator/wai/crawler/wiki/done';
const redisKeyPrefixTodo = '/wator/wai/crawler/wiki/todo';
const redisKeyPrefixIpfs = '/wator/wai/wiki/text';


var client = redis.createClient();

client.keys(redisKeyPrefixIpfs + '/*', function (err, keys) {
  if (err) { 
    return console.log('err=<',err,'>');
  }
  try {
    console.log('keys.length=<',keys.length,'>');
  } catch(e) {
    console.log('err=<',err,'>');
  }
}); 

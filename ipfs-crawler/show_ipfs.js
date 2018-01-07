
/*
var ipfsAPI = require('ipfs-api');
var ipfs = ipfsAPI({host: 'localhost', port: '5001', protocol: 'http'});
var refs = ipfs.refs.local((err,result)=>{
  console.log('err=<',err,'>');
  console.log('result=<',result,'>');
});
console.log('refs=<',refs,'>');
*/


var redis = require('redis');

const redisOption = {
  port:6379,
  host:'master.redis.wator.xyz',
};

var client = redis.createClient(redisOption);

const redisKeyPrefix = '/wator/wai/crawler/wiki';
const redisKeyPrefixDone = '/wator/wai/crawler/wiki/done';
const redisKeyPrefixTodo = '/wator/wai/crawler/wiki/todo';
const redisKeyPrefixIpfs = '/wator/wai/wiki/text';

client.keys(redisKeyPrefixIpfs + '/*', function (err, keys) {
  if (err) { 
    return console.log('err=<',err,'>');
  }
  try {
    console.log(redisKeyPrefixIpfs,'keys.length=<',keys.length,'>');
  } catch(e) {
    console.log('err=<',err,'>');
  }
}); 

client.keys(redisKeyPrefixDone + '/*', function (err, keys) {
  if (err) { 
    return console.log('err=<',err,'>');
  }
  try {
    console.log(redisKeyPrefixDone,'keys.length=<',keys.length,'>');
  } catch(e) {
    console.log('err=<',err,'>');
  }
}); 

client.keys(redisKeyPrefixTodo + '/*', function (err, keys) {
  if (err) { 
    return console.log('err=<',err,'>');
  }
  try {
    console.log(redisKeyPrefixTodo,'keys.length=<',keys.length,'>');
  } catch(e) {
    console.log('err=<',err,'>');
  }
}); 

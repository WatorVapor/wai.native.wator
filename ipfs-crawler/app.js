var ipfsAPI = require('ipfs-api');
var ipfs = ipfsAPI('/ip4/127.0.0.1/tcp/5001');
//console.log('ipfs=<',ipfs,'>');

/*
ipfs.files.add('hello',{},function(err,evt){
  if (err) {
    throw err;
  }
  console.log('evt=<',evt,'>');
});
*/


ipfs.id(function (err, identity) {
  if (err) {
    throw err;
  }
  console.log('identity=<',identity,'>');
});

ipfs.version(function(err, version){
  if (err) {
    throw err;
  }
  console.log(version);
});

ipfs.stats.bitswap(function(err,stat){
  if (err) {
    throw err;
  }
  console.log(stat);
});

ipfs.stats.bw({},function(err,stat){
  if (err) {
    throw err;
  }
  console.log(stat);
});

ipfs.stats.repo({},function(err,stat){
  if (err) {
    throw err;
  }
  console.log(stat);
});


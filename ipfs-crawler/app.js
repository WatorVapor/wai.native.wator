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
  console.log('version=<',version,'>');
});

ipfs.stats.bitswap(function(err,bitswap){
  if (err) {
    throw err;
  }
  console.log('bitswap=<',bitswap,'>');
});

ipfs.stats.bw({},function(err,bw){
  if (err) {
    throw err;
  }
  console.log('bw=<',bw,'>');
});

ipfs.stats.repo({},function(err,repo){
  if (err) {
    throw err;
  }
  console.log('repo=<',repo,'>');
});

ipfs.key.list({},function(err,key){
  if (err) {
    throw err;
  }
  console.log('key=<',key,'>');
});


ipfs.util.addFromFs('/tmp/test.txt',{ recursive: false},function(err, result) {
  if (err) {
    throw err;
  }
  console.log('result=<',result,'>');
});

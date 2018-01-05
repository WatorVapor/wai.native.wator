var ipfsAPI = require('ipfs-api');
var ipfs = ipfsAPI('/ip4/127.0.0.1/tcp/5001');
//console.log('ipfs=<',ipfs,'>');

var data = {test:true};
ipfs.dht.put('hello.11', JSON.stringify(data), function (err) {
  if(err) {
    console.log('err=<',err,'>');
  } else {
    console.log('err=<',err,'>');
  }
});

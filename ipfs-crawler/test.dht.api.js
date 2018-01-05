var ipfsAPI = require('ipfs-api');
var ipfs = ipfsAPI('/ip4/127.0.0.1/tcp/5001');
//console.log('ipfs=<',ipfs,'>');

var key = 'hello.11';
var data = {test:true};
ipfs.dht.put(key, JSON.stringify(data), function (err) {
  if(err) {
    console.log('err=<',err,'>');
  } else {
    console.log('err=<',err,'>');
  }
});

ipfs.dht.get(key, function (err, value) {
  if(err) {
    console.log('err=<',err,'>');
  } else {
    console.log('value=<',value,'>');
  }  
});

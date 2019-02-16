var ipfsAPI = require('ipfs-api');
var ipfs = ipfsAPI('/ip4/127.0.0.1/tcp/5001');
//console.log('ipfs=<',ipfs,'>');

/*
ipfs.id(function (err, identity) {
  if (err) {
    return console.log('err=<',err,'>');
  }
  console.log('identity.id=<',identity.id,'>');
  setTimeout(()=>{
    ipfs.dht.query(identity.id, (err, peerInfos)=> {
      if (err) {
        return console.log('err=<',err,'>');
      }
      console.log('peerInfos=<',peerInfos,'>');
    });
  },1);
})
*/


var key = '9fe394af5829929ade61cc86a3ca6748e8a3c66e0e49ca53430825321d2eaa35215d8ab1514404a0ba1212fa1d71dfb5f4df7536e84306cf27c605a01f6f5fe2';
var data = {test:true};
ipfs.dht.put(key, JSON.stringify(data), function (err) {
  if(err) {
    console.log('put err=<',err,'>');
  } else {
    console.log('put err=<',err,'>');
    ipfs.dht.get(key, { timeout: '10000ms' },function (err, value) {
      if(err) {
        console.log('get err=<',err,'>');
      } else {
        console.log('get value=<',value,'>');
      }  
    });  
  }
});




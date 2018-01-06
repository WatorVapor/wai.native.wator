var ipfsAPI = require('ipfs-api');
var ipfs = ipfsAPI('/ip4/127.0.0.1/tcp/5001');
//console.log('ipfs=<',ipfs,'>');

ipfs.files.ls('/', (err,result) => {
  if (err) {
    return console.log('ipfs.files.ls err=<',err,'>');
  }
  console.log('ipfs.files.ls result=<',result,'>');
});

ipfs.files.stat((err,result) => {
  if (err) {
    return console.log('ipfs.files.stat err=<',err,'>');
  }
  console.log('ipfs.files.stat result=<',result,'>');
});


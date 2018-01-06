var ipfsAPI = require('ipfs-api');
var ipfs = ipfsAPI('/ip4/127.0.0.1/tcp/5001');
//console.log('ipfs=<',ipfs,'>');

ipfs.files.ls('/', msg, (err,result) => {
  if (err) {
    throw err
  }
  console.log('ipfs.files.ls result=<',result,'>');
});

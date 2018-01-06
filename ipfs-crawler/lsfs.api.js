var ipfsAPI = require('ipfs-api');
var ipfs = ipfsAPI('/ip4/127.0.0.1/tcp/5001');
//console.log('ipfs=<',ipfs,'>');

ipfs.files.ls('/', (err,result) => {
  if (err) {
    return console.log('ipfs.files.ls err=<',err,'>');
  }
  console.log('ipfs.files.ls result=<',result,'>');
});

ipfs.key.list((err, keys) => {
  if (err) {
    return console.log('ipfs.key.list err=<',err,'>');
  }
  console.log('ipfs.key.list keys=<',keys,'>');
});

ipfs.files.stat('/',(err,result) => {
  if (err) {
    return console.log('ipfs.files.stat err=<',err,'>');
  }
  console.log('ipfs.files.stat result=<',result,'>');
});

ipfs.refs.local({},(err,result) => {
  if (err) {
    return console.log('ipfs.refs.local err=<',err,'>');
  }
  console.log('ipfs.refs.local result=<',result,'>');
});

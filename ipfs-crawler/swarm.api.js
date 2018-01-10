var ipfsAPI = require('ipfs-api');
//var ipfs = ipfsAPI('/ip4/127.0.0.1/tcp/5001');
var ipfs = ipfsAPI('master.ipfs.wator.xyz', '5001', {protocol: 'http'});
//console.log('ipfs=<',ipfs,'>');

ipfs.swarm.peers(function (err, peerInfos) {
  if (err) {
    throw err;
  }
  console.log('peerInfos=<',peerInfos,'>');
});


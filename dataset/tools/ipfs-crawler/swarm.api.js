var ipfsAPI = require('ipfs-api');
//var ipfs = ipfsAPI('/ip4/127.0.0.1/tcp/5001');
var ipfs = ipfsAPI('master.ipfs.wator.xyz', '5001', {protocol: 'http'});
//var ipfs = ipfsAPI('/ip6/master.ipfs.wator.xyz/tcp/5001');


//console.log('ipfs=<',ipfs,'>');

/*
ipfs.swarm.addrs(function (err, addrs) {
  if (err) {
    throw err;
  }
  console.log('addrs=<',addrs,'>');
})
*/


ipfs.swarm.peers(function (err, peerInfos) {
  if (err) {
    throw err;
  }
  console.log('peerInfos=<',peerInfos,'>');
});


var ipfsAPI = require('ipfs-api');
//var ipfs = ipfsAPI('/ip4/127.0.0.1/tcp/5001');
var ipfs = ipfsAPI('master.ipfs.wator.xyz', '5001', {protocol: 'http'});
//var ipfs = ipfsAPI('/ip6/master.ipfs.wator.xyz/tcp/5001');


//console.log('ipfs=<',ipfs,'>');

const addr = '/ipfs/QmZaddjvL7hd24abTXkedaEa2GCqqb3QPTdA2V6FnN2A24'

ipfs.name.publish(addr, function (err, res) {
  if(err) {
    throw err;
  }
  console.log('res=<',res,'>');
});

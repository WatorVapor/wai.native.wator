const ipfsAPI = require('ipfs-http-client');
//const ipfs = ipfsAPI({host: 'localhost', port: '5003', protocol: 'http'});
const ipfs = ipfsAPI('/ip4/127.0.0.1/tcp/5003');

ipfs.id((err, identity) =>{
  if (err) {
    throw err;
  }
  console.log('ipfs.id identity=<',identity,'>');
});
ipfs.swarm.peers((err, peerInfos)=>{
  if (err) {
    throw err;
  }
  console.log('ipfs.swarm.peers peerInfos=<',peerInfos,'>');  
});

var ipfsAPI = require('ipfs-api');
//var ipfs = ipfsAPI('/ip4/127.0.0.1/tcp/5001');
var ipfs = ipfsAPI('master.ipfs.wator.xyz', '5001', {protocol: 'http'});
//console.log('ipfs=<',ipfs,'>');

const topic = 'fruit-of-the-day-wator';
const msg = new Buffer('banana');
ipfs.pubsub.publish(topic, msg, (err) => {
  if (err) {
    throw err;
  }
  console.log('sented msg=<',msg,'>');
});

ipfs.pubsub.peers(topic, (err, peerIds) => {
  if (err) {
    throw err;
  }
  console.log('peers peerIds=<',peerIds,'>');
})

ipfs.pubsub.ls((err, topics) => {
  if (err) {
    throw err;
  }
  console.log('ls topics=<',topics,'>');
})


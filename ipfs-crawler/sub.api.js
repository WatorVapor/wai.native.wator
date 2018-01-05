var ipfsAPI = require('ipfs-api');
var ipfs = ipfsAPI('/ip4/127.0.0.1/tcp/5001');
//console.log('ipfs=<',ipfs,'>');

const topic = 'fruit-of-the-day-wator';
const receiveMsg = (msg) => {
  console.log('receiveMsg msg=<',msg,'>');
}
ipfs.pubsub.subscribe(topic, receiveMsg,(err) => {
  if (err) {
    throw err
  }
  console.log('subscribe topic=<',topic,'>');
});

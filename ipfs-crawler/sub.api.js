var ipfsAPI = require('ipfs-api');
var ipfs = ipfsAPI('/ip4/127.0.0.1/tcp/5001');
//console.log('ipfs=<',ipfs,'>');

const topic = 'fruit-of-the-day';
const msg = new Buffer('banana');
ipfs.pubsub.publish(topic, msg, (err) => {
  if (err) {
    throw err
  }
  console.log('sented msg=<',msg,'>');
});

const ipfsAPI = require('ipfs-http-client');
const strConstAddress = '/ip4/127.0.0.1/tcp/5011';

const node = ipfsAPI(strConstAddress);

node.refs.local((err,result) => {
  if(err) {
    throw err;
  }
  console.log('node.refs.local: result=<',result,'>');
})

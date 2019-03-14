const ipfsAPI = require('ipfs-http-client');
const fs=require("fs");

const strConstAddress = '/ip4/127.0.0.1/tcp/5011';

const node = ipfsAPI(strConstAddress);

node.refs.local((err,result) => {
  if(err) {
    throw err;
  }
  console.log('node.refs.local: result=<',result,'>');
  fs.writeFileSync('./refs.local.json',JSON.stringify(result,undefined,2));
})

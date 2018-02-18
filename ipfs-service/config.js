const ipfsAPI = require('ipfs-api');
const ipfs = ipfsAPI('/ip4/127.0.0.1/tcp/5001');
//var ipfs = ipfsAPI('master.ipfs.wator.xyz', '5001', {protocol: 'http'});

ipfs.config.set('Datastore.StorageMax','16GB',(err, reply) => {
  if (err) {
    throw err
  }
  console.log('get::reply=<',reply,'')
})


ipfs.config.get('Datastore.StorageMax',(err, config) => {
  if (err) {
    throw err
  }
  console.log('get::config=<',config,'')
})

let methods = ['GET', 'HEAD', 'OPTIONS', 'POST'];
JSON.stringify(methods);

ipfs.config.set('Gateway.HTTPHeaders.Access-Control-Allow-Methods',methods,(err, reply) => {
  if (err) {
    throw err
  }
  console.log('get::reply=<',reply,'')
})

ipfs.config.get('Gateway.HTTPHeaders.Access-Control-Allow-Methods',(err, config) => {
  if (err) {
    throw err
  }
  console.log('get:: Gateway.HTTPHeaders.Access-Control-Allow-Methods config=<',config,'')
})



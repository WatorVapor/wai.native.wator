var ipfsAPI = require('ipfs-api');
var ipfs = ipfsAPI('/ip4/127.0.0.1/tcp/5001');
//var ipfs = ipfsAPI('master.ipfs.wator.xyz', '5001', {protocol: 'http'});

ipfs.config.set('Datastore.StorageMax','15GB',(err, reply) => {
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

const argv = require('argv');
const args = argv.run();
const pathDB = args.targets[0];
console.log('pathDB =<',pathDB,'>');

const level = require('level');

const db = level(pathDB);
const stream = db.createReadStream();

let counter = 0;
let gBlockPrevReferred = {};
let gFirstBlock = false;

stream.on('data', function (data) {
  //console.log(data.key.toString('utf-8'), '=', data.value.toString('utf-8'));
  //console.log('data.key=<',data.key.toString('utf-8'),'>')
  //console.log('data.value=<',data.value.toString('utf-8'),'>')
  counter++
  stream.pause();
  getBlockFromIpfs(data.key.toString('utf-8'));
});
stream.on('error', function (err) {
  console.log('error err=<',err,'>');
});
stream.on('close', function () {
  console.log('Stream closed');
  console.log('gFirstBlock=<',gFirstBlock,'>');
  //console.log('gBlockPrevReferred=<',gBlockPrevReferred,'>');
  for (let blockCID in gBlockPrevReferred) {
    //console.log('blockCID=<',blockCID,'>');
    if(!gBlockPrevReferred[blockCID]) {
      console.log('lastblock CID=<',blockCID,'>');
    }
  }
});
stream.on('end', function () {
  console.log('Stream ended');
  console.log('counter=<',counter,'>');
});

const ipfsAPI = require('ipfs-http-client');
const ipfs = ipfsAPI({host: 'localhost', port: '5011'});
ipfs.id(function (err, identity) {
  if (err) {
    throw err;
  }
  console.log('ipfs.id identity=<',identity,'>');
});

async function getBlockFromIpfs(cid) {
  //console.log('getBlockFromIpfs cid=<',cid,'>');  
  try {
    let result = await ipfs.get(cid);
    //console.log('getBlockFromIpfs::result=<',result,'>');
    if(result) {
      if(result.length === 1 ) {
        let file = result[0];
        onIpfsBlockContent(file,cid);
      } else {
        if(result.length > 1) {
          //console.log('getBlockFromIpfs:: directory ?? cid=<',cid,'>');
        } else {
          console.log('getBlockFromIpfs:: error?? cid=<',cid,'>');          
        }
        stream.resume();      
      }
    } else {
      console.log('getBlockFromIpfs::result=<',result,'>');
      stream.resume();      
    }
  }catch (e) {
    console.error('getBlockFromIpfs::e=<',e,'>');
    console.error('getBlockFromIpfs:: cid=<',cid,'>');          
    stream.resume();
  }

}


function onIpfsBlockContent(file,cid) {
  //console.log('onIpfsBlockContent::file=<',file,'>');
  //console.log('onIpfsBlockContent::file.path=<',file.path,'>');
  //console.log('onIpfsBlockContent::file.content.length=<',file.content.length,'>');
  if(file.content) {
    let blockJson = JSON.parse(file.content);
    //console.log('onIpfsBlockContent::blockJson=<',blockJson,'>');
    //console.log('onIpfsBlockContent::cid=<',cid,'>');
    let prev = blockJson.prev;
    //console.log('onIpfsBlockContent::prev=<',prev,'>');
    if(!gBlockPrevReferred[prev]) {
      gBlockPrevReferred[prev] = true;
    }
    if(!gBlockPrevReferred[cid]) {
      gBlockPrevReferred[cid] = false;
    }
    if( prev === 'Genesis') {
      gFirstBlock = cid;
    }
 }
  stream.resume();
}


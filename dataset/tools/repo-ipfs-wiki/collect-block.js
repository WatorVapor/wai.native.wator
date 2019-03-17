const dbPath = './cnwiki-cid';
const dbBlockPath = './cnwiki-block';
const crypto = require('crypto');
const hash = crypto.createHash('sha256');
hash.update('wai text wiki cn');
const gGroup = hash.digest('hex');


const level = require('level');
const ipfsAPI = require('ipfs-http-client');

const ipfs = ipfsAPI({host: 'localhost', port: '5011'});


ipfs.id(function (err, identity) {
  if (err) {
    throw err;
  }
  console.log('ipfs.id identity=<',identity,'>');
});


let prevBlock = 'Genesis';
let lastCollectedClip = '';
let dbBlock = level(dbBlockPath);
const topBlockPosition = 'top-block-position';
dbBlock.get(topBlockPosition, function (err, value) {
  if (err && err.notFound) {
    setTimeout(function(){
      startReadClips();
    },1);
  } else {
    console.log('topBlockPosition value=<',value,'>');
    prevBlock = value;
    setTimeout(function(){
      dbBlock.get(value, function (err2, value2) {
        if (err2) {
        } else {
          console.log('topBlockPosition value2=<',value2,'>');
          lastCollectedClip = value2;
          setTimeout(function(){
            startReadClips();
          },1);
        }
      });
    },1);
  }
});

let db = level(dbPath);
let stream = false;
function startReadClips() {
  console.log('streamBlock prevBlock=<',prevBlock,'>');
  console.log('streamBlock lastCollectedClip=<',lastCollectedClip,'>');
  if(lastCollectedClip) {
    stream = db.createReadStream({gt:lastCollectedClip});
  } else {
    stream = db.createReadStream();
  }
  stream.on('data', function (data) {
    //console.log(data.key.toString('utf-8'), '=', data.value.toString('utf-8'));
    //console.log('data.key=<',data.key.toString('utf-8'),'>');
    //console.log('data.value=<',data.value.toString('utf-8'),'>');
    if(data.key.toString('utf-8') !== 'wiki_dump_resume_pos') {
      stream.pause();
      readIpfsInfo(data.key.toString('utf-8'));
    }
  });
  stream.on('error', function (err) {
    console.log('Oh my!', err);
  });
  stream.on('close', function () {
    console.log('Stream closed');
  });
  stream.on('end', function () {
    console.log('Stream ended');
    writeBlock();
  });
}


let blockSizeCounter = 0;
let blockResourceCache = [];
const OneBlockSize = 4*1024*1024;
const minArticleSize = 100;
function readIpfsInfo(path) {
  //console.log('readIpfsInfo::path=<',path,'>');
  ipfs.get(path, function (err, files) {
    //console.log('readIpfsInfo::files=<',files,'>');
    if(err) {
      console.log('readIpfsInfo::files=<',files,'>');
      console.log('readIpfsInfo::err=<',err,'>');
      throw err;
    }
    try {
      files.forEach((file) => {
        //console.log('readIpfsInfo::file=<',file,'>');
        //console.log('readIpfsInfo::file.path=<',file.path,'>');
        //console.log('readIpfsInfo::file.content.length=<',file.content.length,'>');
          if(file.content && file.content.length > minArticleSize) {
            blockSizeCounter += file.content.length;
            blockResourceCache.push(file.path);
            if(blockSizeCounter >= OneBlockSize) {
              writeBlock(path);
            } else {
              stream.resume();
            }
          } else {
            stream.resume();
          }
      });
    } catch(e) {
      console.log('readIpfsInfo::e=<',e,'>');
      console.log('readIpfsInfo::files=<',files,'>');
    }
  });  
}




function writeBlock(path) {
  //console.log('writeBlock::blockSizeCounter=<',blockSizeCounter,'>');
  //console.log('writeBlock::blockResourceCache=<',blockResourceCache,'>');
  let block = {};
  block.prev = prevBlock;
  block.group = gGroup;
  block.size = blockSizeCounter;
  block.resource = blockResourceCache;
  let blockStr = JSON.stringify(block);
  //console.log('writeBlock::blockStr=<',blockStr,'>');
  let bufBlock = Buffer.from(blockStr, 'utf8');
  save2Ipfs(bufBlock,path);
  
  blockSizeCounter = 0;
  blockResourceCache = [];
}

async function pushIpfs2BlockDB(key,value) {
  //console.log('pushIpfs2BlockDB::key=<',key,'>');
  //console.log('pushIpfs2BlockDB::value=<',value,'>');
  await dbBlock.put(key,value);
}


async function save2Ipfs(bufBlock,path){
  let result = await ipfs.add(bufBlock);
  //console.log('save2Ipfs::result=<',result,'>');
  let hash = result[0].hash;
  console.log('save2Ipfs::hash=<',hash,'>');
  prevBlock = hash;
  //console.log('save2Ipfs::cnTitle=<',cnTitle,'>');
  //console.log('save2Ipfs::pos=<',pos,'>');
  if(path) {
    pushIpfs2BlockDB(hash,path);
  }
  pushIpfs2BlockDB(topBlockPosition,hash);
  if(hash) {
     stream.resume();
  }
} 


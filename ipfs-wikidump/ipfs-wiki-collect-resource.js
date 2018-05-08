const dbPath = '/watorvapor/wai.storage/dumps.wikimedia.org/output_leveldb/cnwiki/ipfs';

const level = require('level');
const ipfsAPI = require('ipfs-api');
const ipfs = ipfsAPI({host: 'localhost', port: '5002', protocol: 'http'});

ipfs.id(function (err, identity) {
  if (err) {
    throw err;
  }
  console.log('ipfs.id identity=<',identity,'>');
});



let db = level(dbPath);
let stream = db.createReadStream();
var counter = 0;
stream.on('data', function (data) {
  //console.log(data.key.toString('utf-8'), '=', data.value.toString('utf-8'));
  console.log('data.key=<',data.key.toString('utf-8'),'>');
  console.log('data.value=<',data.value.toString('utf-8'),'>');
  stream.pause();
  readIpfsInfo(data.key.toString('utf-8'));
  counter++
});
stream.on('error', function (err) {
  console.log('Oh my!', err)
});
stream.on('close', function () {
  console.log('Stream closed')
});
stream.on('end', function () {
  console.log('Stream ended')
  console.log('counter=<',counter,'>')
});


let blockSizeCounter = 0;
let blockResourceCache = [];
function readIpfsInfo(path) {
  console.log('readIpfsInfo::path=<',path,'>');
  
  ipfs.files.get(path, function (err, files) {
    //console.log('readIpfsInfo::files=<',files,'>');
    files.forEach((file) => {
      //console.log('readIpfsInfo::file=<',file,'>');
      //console.log('readIpfsInfo::file.content.length=<',file.content.length,'>');
      blockSizeCounter += file.content.length;
      blockResourceCache.push(file.path);
      if(blockSizeCounter >= 1024*1024) {
        writeBlock();
      } else {
        stream.resume();
      }
    });
  });  
}


const crypto = require('crypto');
const hash = crypto.createHash('sha256');
hash.update('wai text wiki cn');
const gGroup = hash.digest('hex');
let prevBlock = 'Genesis';


function writeBlock() {
  //console.log('writeBlock::blockSizeCounter=<',blockSizeCounter,'>');
  //console.log('writeBlock::blockCache=<',blockCache,'>');
  let block = {};
  block.prev = prevBlock;
  block.group = gGroup;
  block.size = blockSizeCounter;
  block.resource = blockResourceCache;
  let blockStr = JSON.stringify(block);
  console.log('writeBlock::blockStr=<',blockStr,'>');
  blockSizeCounter = 0;
  blockCache = [];
}

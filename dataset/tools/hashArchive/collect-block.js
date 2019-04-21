const shake256=require('js-sha3').shake256;
const sha3_224=require('js-sha3').sha3_224;
const bs58 = require('bs58');
const fs=require("fs");
const { execSync } = require('child_process');

const hashInputRoot = '/ceph/storage3/hashArchive/cnwiki/201903'
const blockRoot = '/ceph/storage3/hashArchive/cnwiki/block_201903';

let resume = {};

const memNewFiles = [];

onNewDir = (rootDir,leaf) => {
  const dirs = fs.readdirSync(rootDir);
  //console.log('onNewDir:: dirs=<',dirs,'>');
  for(let dir of dirs) {
    //console.log('onNewDir:: dir=<',dir,'>');
    let fullPath = rootDir + '/' + dir;
    if (fs.statSync(fullPath).isDirectory()) {
      onNewDir(fullPath,dir);
    }
    if (fs.statSync(fullPath).isFile()) {
      onNewFile(rootDir,dir);
    }
  }
}




/*
setTimeout(()=> {
  onNewDir(hashInputRoot,'ffff');
},1)
*/


addressOfContent = (content) => {
  let shake = shake256(content,160);
  //console.log('addressOfContent shake=<',shake,'>');
  const bytes = Buffer.from(shake,'hex');
  const address = bs58.encode(bytes);
  //console.log('addressOfContent address=<',address,'>');  
  return address;
}


const errorReport = {
  badFiles:[],
};


onNewFile = (path,name) => {
  //console.log('onNewFile:: path=<',path,'>');
  //console.log('onNewFile:: name=<',name,'>');
  let filePath = path + '/' + name;
  //console.log('onNewFile:: filePath=<',filePath,'>');
  let content = fs.readFileSync(filePath, 'utf8');
  //console.log('onNewFile:: content=<',content,'>');
  //console.log('onNewFile:: content.length=<',content.length,'>');
  //console.log('onNewFile:: Buffer.byteLength(content)=<',Buffer.byteLength(content),'>');
  let address = addressOfContent(content);
  //console.log('onNewFile:: address=<',address,'>');
  if(address === name) {
    onFileContent(content,name);
  } else {
    console.log('error!!! onNewFile:: address=<',address,'>');
    console.log('error!!! onNewFile:: name=<',name,'>');
    errorReport.badFiles.push({address:address,name:name});
    let errorReportStr = JSON.stringify(errorReport,undefined,2);
    fs.writeFileSync('./errorReport.json',errorReportStr);
  }
}


let prevBlock = 'Genesis';
let blockSizeCounter = 0;
let blockResourceCache = [];
const OneBlockSize = 4*1024*1024;
const minArticleSize = 100;

const gGroupMsg = 'wai text wiki cn for carrot';
const gGroup = sha3_224(gGroupMsg);



onFileContent = (content,name) =>{
  //console.log('onFileContent::content=<',content,'>');
  //console.log('onFileContent::name=<',name,'>');
  const length = Buffer.byteLength(content);
  //console.log('onFileContent::length=<',length,'>');
  if(length > minArticleSize) {
    blockSizeCounter += length;
    blockResourceCache.push(name);
    if(blockSizeCounter >= OneBlockSize) {
      writeBlock();
    }
  }  
}

const DEPTH = 3;
const HASH_DIR_STEP = 2;

const blockChain = {
  msg:gGroupMsg,
  group:gGroup,
  block:[
    prevBlock
  ],
  tail:prevBlock
};

writeBlock = () => {
  //console.log('writeBlock::blockSizeCounter=<',blockSizeCounter,'>');
  //console.log('writeBlock::blockResourceCache=<',blockResourceCache,'>');
  let block = {};
  block.prev = prevBlock;
  block.group = gGroup;
  block.size = blockSizeCounter;
  block.resource = blockResourceCache;
  //let blockStr = JSON.stringify(block,undefined,2);
  let blockStr = JSON.stringify(block,undefined,2);
  //console.log('writeBlock::blockStr=<',blockStr,'>');
  let blockAddress = addressOfBlock(blockStr);
  blockSizeCounter = 0;
  blockResourceCache = [];
  prevBlock = blockAddress;
  console.log('writeBlock::blockAddress=<',blockAddress,'>');
  let deepHash = blockRoot;
  for(let i = 0 ;i < DEPTH;i++) {
    deepHash += '/' +  blockAddress.slice(i* HASH_DIR_STEP,(i+1) * HASH_DIR_STEP);
    //console.log('writeBlock:: deepHash=<',deepHash,'>');
    if(!fs.existsSync(deepHash)) {
      fs.mkdirSync(deepHash)
    }
  }
  console.log('writeBlock:: deepHash=<',deepHash,'>');
  let blockOut = deepHash + '/' + blockAddress;
  console.log('writeBlock::blockOut=<',blockOut,'>');
  fs.writeFileSync(blockOut,blockStr);
  blockChain.block.push(blockAddress);
  blockChain.tail = blockAddress;
  let chainStr = JSON.stringify(blockChain,undefined,2);
  fs.writeFileSync('./blockchain.json',chainStr);
}



addressOfBlock = (blockStr) => {
  return addressOfContent(blockStr);
}


onNewDir(hashInputRoot,'ffff');
console.log('last::blockChain=<',blockChain,'>');
writeBlock();
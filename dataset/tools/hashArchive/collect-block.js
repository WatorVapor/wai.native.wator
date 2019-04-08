const shake256=require('js-sha3').shake256;
const sha3_224=require('js-sha3').sha3_224;
const bs58 = require('bs58');
const fs=require("fs");
const { execSync } = require('child_process');

const hashInputRoot = '/ceph/storage3/hashArchive/cnwiki/201903'
const blockRoot = '/ceph/storage3/hashArchive/cnwiki/block_201903';

let resume = {};



onNewDir = (rootDir,leaf) => {
  /*
  if(resume && resume.top) {
    let resumeTop = parseInt(resume.top,16);
    let leafNum = parseInt(leaf,16);
    console.log('onNewDir:: resumeTop=<',resumeTop,'>');
    console.log('onNewDir:: leafNum=<',leafNum,'>');
    if(resumeTop > leafNum) {
      console.log('onNewDir:: skip finnished resumeTop > leafNum=<',resumeTop > leafNum,'>');
      return;
    }
  }
  */
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

setTimeout(()=> {
  onNewDir(hashInputRoot,'ffff');
},1)


addressOfContent = (content) => {
  let shake = shake256(content,160);
  //console.log('addressOfContent shake=<',shake,'>');
  const bytes = Buffer.from(shake,'hex');
  const address = bs58.encode(bytes);
  //console.log('addressOfContent address=<',address,'>');  
  return address;
}



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
    console.log('onNewFile:: address=<',address,'>');
    console.log('onNewFile:: name=<',name,'>');
    process.exit(-1);
  }

}


let prevBlock = 'Genesis';
let blockSizeCounter = 0;
let blockResourceCache = [];
const OneBlockSize = 4*1024*1024;
const minArticleSize = 100;

///const hash = crypto.createHash('sha256');
//hash.update('wai text wiki cn');
const gGroup = sha3_224('wai text wiki cn');



onFileContent = (content,name) =>{
  //console.log('onFileContent::content=<',content,'>');
  //console.log('onFileContent::name=<',name,'>');
  const length = Buffer.byteLength(content);
  //console.log('onFileContent::length=<',length,'>');
  if(length > minArticleSize) {
    blockSizeCounter += length;
    blockResourceCache.push(name);
    if(blockSizeCounter >= OneBlockSize) {
      writeBlock(name);
    }
  }  
}

writeBlock = (path) => {
  //console.log('writeBlock::blockSizeCounter=<',blockSizeCounter,'>');
  //console.log('writeBlock::blockResourceCache=<',blockResourceCache,'>');
  let block = {};
  block.prev = prevBlock;
  block.group = gGroup;
  block.size = blockSizeCounter;
  block.resource = blockResourceCache;
  //let blockStr = JSON.stringify(block,undefined,2);
  let blockStr = JSON.stringify(block,undefined,2);
  console.log('writeBlock::blockStr=<',blockStr,'>');
  let bufBlock = Buffer.from(blockStr, 'utf8');
  save2Ipfs(bufBlock,path);
  
  blockSizeCounter = 0;
  blockResourceCache = [];
}

save2Ipfs = async (bufBlock,name) => {
  let result = await ipfs.add(bufBlock,{onlyHash:true});
  console.log('save2Ipfs::result=<',result,'>');
}

const IPFS = require('ipfs');
const node = new IPFS();






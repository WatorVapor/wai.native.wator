const shake256=require('js-sha3').shake256;
const bs58 = require('bs58');
/*
//console.log('shake256 =<',shake256 ,'>');
let name = shake256('Message to hash',128);
//console.log('name=<',name,'>');
const bytes = Buffer.from(name,'hex');
const address = bs58.encode(bytes);
console.log('address=<',address,'>');
*/
const fs=require("fs");
const { execSync } = require('child_process');

const DEPTH = 2;
const HASH_DIR_STEP = 3;

const hashInputRoot = '/watorvapor/wai.storage/old.hashArchive'
const hashRoot = '/watorvapor/wai.storage/hashArchive';

onNewDir = (rootDir) => {
  const dirs = fs.readdirSync(rootDir);
  //console.log('onNewDir:: dirs=<',dirs,'>');
  for(let dir of dirs) {
    //console.log('onNewDir:: dir=<',dir,'>');
    let fullPath = rootDir + '/' + dir;
    if (fs.statSync(fullPath).isDirectory()) {
      onNewDir(fullPath);
    }
    if (fs.statSync(fullPath).isFile()) {
      onNewFile(rootDir,dir);
    }
  } 
}

onNewFile = (path,name) => {
  console.log('onNewFile:: path=<',path,'>');
  console.log('onNewFile:: name=<',name,'>');
/*  
  let depths = path.replace(hashRoot,'').split('/').filter(v=>v!='');
  //console.log('onNewFile:: depths=<',depths,'>');
  if(depths.length >= DEPTH) {
    console.log('onNewFile:: depths=<',depths,'>');
    return;
  }
  let deepHash = hashRoot;
  for(let i = 0 ;i < DEPTH;i++) {
    deepHash += '/' +  name.slice(0,(i+1) * HASH_DIR_STEP);
    //console.log('onNewFile:: deepHash=<',deepHash,'>');
    if(!fs.existsSync(deepHash)) {
      fs.mkdirSync(deepHash)
    }
  }
  //console.log('onNewFile:: deepHash=<',deepHash,'>');
  let newPath = deepHash + '/' + name;
  let oldPath = path + '/' + name;
  //console.log('onNewFile:: newPath=<',newPath,'>');
  //console.log('onNewFile:: oldPath=<',oldPath,'>');
  let move = 'mv ' + oldPath + ' ' + newPath;
  console.log('onNewFile:: move=<',move,'>');
  const stdout = execSync(move);
  //console.log('onNewFile:: stdout=<',stdout,'>');
*/
}

onNewDir(hashInputRoot);

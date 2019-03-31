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

const DEPTH = 3;
const HASH_DIR_STEP = 2;

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
  let address = addressOfContent(content);
  //console.log('onNewFile:: address=<',address,'>');
  let deepHash = hashRoot;
  for(let i = 0 ;i < DEPTH;i++) {
    deepHash += '/' +  address.slice(i* HASH_DIR_STEP,(i+1) * HASH_DIR_STEP);
    //console.log('onNewFile:: deepHash=<',deepHash,'>');
    if(!fs.existsSync(deepHash)) {
      fs.mkdirSync(deepHash)
    }
  }
  //console.log('onNewFile:: deepHash=<',deepHash,'>');
  let newPath = deepHash + '/' + address;
  console.log('onNewFile:: newPath=<',newPath,'>');
  if(!fs.existsSync(newPath)) {
    fs.writeFileSync(newPath,content);
  }
}

onNewDir(hashInputRoot);

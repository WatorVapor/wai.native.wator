const fs=require("fs");
const { execSync } = require('child_process');
const hashRoot = '/watorvapor/wai.storage/hashArchive';
const DEPTH = 2;
const HASH_DIR_STEP = 3;

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
  //console.log('onNewFile:: path=<',path,'>');
  //console.log('onNewFile:: name=<',name,'>');
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
}

onNewDir(hashRoot);


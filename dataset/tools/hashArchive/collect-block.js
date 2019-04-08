const shake256=require('js-sha3').shake256;
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

onNewFile = (path,name) => {
  //console.log('onNewFile:: path=<',path,'>');
  //console.log('onNewFile:: name=<',name,'>');
  let filePath = path + '/' + name;
  console.log('onNewFile:: filePath=<',filePath,'>');
  let content = fs.readFileSync(filePath, 'utf8');
  console.log('onNewFile:: content=<',content,'>');
  console.log('onNewFile:: content.length=<',content.length,'>');

}

onNewDir(hashInputRoot,'ffff');

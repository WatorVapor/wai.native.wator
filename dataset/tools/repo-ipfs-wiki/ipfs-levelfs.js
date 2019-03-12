const fs=require('fs');
const crypto = require('crypto');
const execSync= require('child_process').execSync;

module.exports = class IpfsSaveFs {
  constructor(path) {
    this._path = path;
    const result = execSync('mkdir -p ' + this._path);
    console.log('IpfsSaveFs::constructor result=<',result,'>');
    let self = this;
    setTimeout( () => {
      if(typeof self.onReady === 'function') {
        self.onReady();
      }
    },1);
  }
  saveFS(cnTitle,cnText,pos,cb) {
    const shasum = crypto.createHash('sha1');
    shasum.update(cnText);
    let hash = shasum.digest('hex');
    //console.log('IpfsSaveFs::saveFS hash=<',hash,'>');
    let path = this._path + '/' + hash.substring(0, 3) + '/';
    //console.log('IpfsSaveFs::saveFS path=<',path,'>');
    const result =  execSync('mkdir -p ' + path);
    //console.log('IpfsSave::saveFS result=<',result,'>');
    let textPath = path + hash;
    if(fs.existsSync(textPath)) {
       console.log('IpfsSaveFs::saveFS !!!!! not good textPath=<',textPath,'>');
       //fs.writeFileSync(textPath + '_2',cnText);
       console.log('IpfsSaveFs::saveFS IpfsSave::save cnTitle=<',cnTitle,'>');
    } else {
       //console.log('IpfsSaveFs::saveFS textPath=<',textPath,'>');
       fs.writeFileSync(textPath,cnText);
    }
    cb(hash,true);
  }
}

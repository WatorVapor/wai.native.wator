const fs=require('fs');
const crypto = require('crypto');
const execSync= require('child_process').execSync;

module.exports = class IpfsSaveFs {
  constructor(path) {
    this._path = path;
    const result = execSync('mkdir -p ' + this._path);
    console.log('IpfsSave::constructor result=<',result,'>');
    let self = this;
    setTimeout( () => {
      if(typeof self.onReady === 'function') {
        self.onReady();
      }
    },1);
  }
  saveFS(cnTitle,cnText,pos,cb) {
    const shasum = crypto.createHash('sha1');
    shasum.update(cnTitle);
    let hash = shasum.digest('hex');
    let path = this._path + hash.substring(0, 6) + '/';
    const result =  execSync('mkdir -p ' + hash);
    console.log('IpfsSave::save result=<',result,'>');
    fs.writeFileSync(path + 'text',cnText);
    cb(hash,true);
  }
}

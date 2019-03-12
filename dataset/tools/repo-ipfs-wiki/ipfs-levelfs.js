
module.exports = class IpfsSaveFs {
  constructor(path) {
    this._path = path;
    const result = execSync('mkdir -p ' + this._path);
    console.log('IpfsSave::constructor result=<',result,'>');
  }
  saveFS(cnTitle,cnText,pos,cb) {
    const shasum = crypto.createHash('sha1');
    shasum.update(cnTitle);
    let hash = shasum.digest('hex');
    let path = this._path + hash + '/';
    const result =  execSync('mkdir -p ' + hash);
    console.log('IpfsSave::save result=<',result,'>');
    fs.writeFileSync(path + 'title',cnTitle);
    fs.writeFileSync(path + 'text',cnText);
    fs.writeFileSync(path + 'pos',pos);
    let self = this;
    this.tempDirCounter++;
    if(this.tempDirCounter < 100) {
      cb('1111',true);
    } else {
      setTimeout(()=>{
        self._tmpFolderCheck();
      },1000);      
    }
  }
}

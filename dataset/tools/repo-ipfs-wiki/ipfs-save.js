const IPFS = require('ipfs');
const config = { 
  repo: './repos_ipfs_public',
  config: {
    Addresses: {
      Swarm: [
        '/ip6/::/tcp/4006'
      ]
    },
    relay: {
      enabled: false, hop: { enabled: false, active: false }
    },
    Discovery: {
      MDNS: {
        Enabled: true,
        Interval: 10
      },
      webRTCStar: {
        Enabled: true
      }
    },
    Bootstrap: [
    ],
    Swarm: {
      ConnMgr: {
        LowWater: 600,
        HighWater: 900
      }
    }
  }
};

const execSync= require('child_process').execSync;
const fs=require('fs');
const crypto = require('crypto');
const strConstTempDir = './tmp_files/';

module.exports = class IpfsSave {
  constructor() {
    this._tryOpenIpfsNode();
    this.cacheToSave = [];
    
    const result = execSync('rm -rf ./tmp_files/ ; mkdir -p ./tmp_files/');
    console.log('IpfsSave::constructor result=<',result,'>');
    this.tempDirCounter = 0;
    this.isSaving = false;
  }
  saveFS(cnTitle,cnText,pos,cb) {
    const shasum = crypto.createHash('sha1');
    shasum.update(cnTitle);
    let hash = shasum.digest('hex');
    let path = './tmp_files/' + hash + '/'
    const result =  execSync('mkdir -p ./tmp_files/' + hash);
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
  _tmpFolderCheck() {
    console.log('IpfsSave::_tmpFolderCheck this.tempDirCounter=<',this.tempDirCounter,'>');
    if(this.tempDirCounter >= 100 && this.isSaving === false) {
      this._saveFromFS();
    }
  }
  _saveFromFS() {
    this.isSaving = true;
    console.log('IpfsSave::_saveFromFS this.tempDirCounter=<',this.tempDirCounter,'>');
    let self = this;
    this.lastSave = new Date();
    this.node.addFromFs(strConstTempDir,{ recursive: true},(err, result) =>{
      if (err) {
        throw err;
      }
      console.log('IpfsSave::_saveFromFS result=<',result,'>');
      self.isSaving = false;
    })
  }
  
  save(cnTitle,cnText,pos,cb) {
    if(this.isSaving) {
      let cache = {
        cnTitle:cnTitle,
        cnText:cnText,
        pos:pos,
        cb:cb
      };
      this.cacheToSave.push(cache);
      return;
    }
    let bufText = Buffer.from(cnText, 'utf8');
    let self = this;
    //console.log('IpfsSave::save pos=<',pos,'>');
    //console.log('IpfsSave::save cnText.length=<',cnText.length,'>');
    
    this.lastSave = new Date();
    this.isSaving = true;
    
    this.node.add(bufText,(err, result) =>{
      if (err) {
        console.log('save::typeof self.onError=<',typeof self.onError,'>');
        if(typeof self.onError === 'function') {
          self.onError(err);
        }
        return;
      }
      self.isSaving = false;
      self.lastSave = new Date();
      //console.log('IpfsSave::save result=<',result,'>');
      let hash = result[0].hash;
      //console.log('IpfsSave::save hash=<',hash,'>');
      //console.log('IpfsSave::save cnTitle=<',cnTitle,'>');
      //console.log('IpfsSave::save pos=<',pos,'>');
      if(hash && typeof cb === 'function') {
        if(self.cacheToSave.length > 0) {
          cb(hash,false);
          self._popCache();
        } else {
          cb(hash,true);
        }
      }
    });
  }
  _popCache() {
    if(this.cacheToSave.length > 0) {
      let cache = this.cacheToSave[0];
      this.save(cache.cnTitle,cache.cnText,cache.pos,cache.cb);
      this.cacheToSave.shift();
    }
  }
  _tryOpenIpfsNode (cb) {
    this.node = new IPFS(config);
    let self = this;
    this.node.on('ready', () => {
      if(typeof self.onReady === 'function') {
        self.onReady();
      }
      setTimeout(()=>{
        self._watchIPFSStatus();
      },1000);      
    });
  }
  _watchIPFSStatus() {
    let now = new Date();
    console.log('_watchIPFSStatus now - this.lastSave=<',now - this.lastSave,'>');
    /*
    let self = this;
    this.node.id((err, identity) => {
      if (err) {
        console.log('_watchIPFSStatus err=<',err,'>');
        if(typeof self.onError === 'function') {
          self.onError(err);
        }
      }
    });
    setTimeout(()=>{
      self._watchIPFSStatus();
    },1000*10);
    */
  }
  _restartIpfs() {
  }
}

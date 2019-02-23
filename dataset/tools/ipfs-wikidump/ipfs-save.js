const ipfsAPI = require('ipfs-http-client');
const strConstAddress = '/ip4/127.0.0.1/tcp/5001';
const iConstSavePageOneTime = 1024;
const execSync= require('child_process').execSync;

module.exports = class IpfsSave {
  constructor() {
    this._tryOpenIpfsNode();
  }
  save(cnTitle,cnText,pos,cb) {
    let bufText = Buffer.from(cnText, 'utf8');
    let self = this;
    this.node.add(bufText,(err, result) =>{
      if (err) {
        console.log('_tryOpenIpfsNode::typeof self.onError=<',typeof self.onError,'>');
        if(typeof self.onError === 'function') {
          self.onError(err);
        }
        return;
      }
      //console.log('IpfsSave::save result=<',result,'>');
      let hash = result[0].hash;
      //console.log('IpfsSave::save hash=<',hash,'>');
      //console.log('IpfsSave::save cnTitle=<',cnTitle,'>');
      //console.log('IpfsSave::save pos=<',pos,'>');
      if(hash && typeof cb === 'function') {
        cb(hash);
      }
    });
  }
  _tryOpenIpfsNode (cb) {
    this.node = ipfsAPI(strConstAddress);
    this.iSavePageCounter = 0;
    let self = this;
    this.node.id((err, identity) => {
      if (err) {
        return;
      }
      //console.log('_tryOpenIpfsNode::this.node=<',this.node,'>');
      console.log('_tryOpenIpfsNode identity=<',identity,'>');
      if(self.remainText && self.remainTitle && self.remainPos) {
        self.save(self.remainTitle,self.remainText,self.remainPos);
      }
      setTimeout(()=>{
        self._watchIPFSStatus();
      },1000)
    });
  }
  _watchIPFSStatus() {
    try {
      let deamonStatus = execSync('docker stats --no-stream | grep go-crystal_ipfs-public').toString('utf-8');
      //console.log('_watchIPFSStatus deamonStatus=<',deamonStatus,'>');
      let deamonStatusVect = deamonStatus.split('%');
      //console.log('_watchIPFSStatus deamonStatusVect=<',deamonStatusVect,'>');
      if(deamonStatusVect.length > 1) {
        //console.log('_watchIPFSStatus deamonStatusVect[1]=<',deamonStatusVect[1],'>');
        let memVect = deamonStatusVect[1].split('iB');
        //console.log('_watchIPFSStatus memVect=<',memVect,'>');
        if(memVect.length > 2) {
          let mem = parseFloat(memVect[2].trim());
          console.log('_watchIPFSStatus mem=<',mem,'>');
          if(mem > 80.0) {
            this._restartIpfs();
          }
        }
      }
      let self = this;
      setTimeout(()=>{
        self._watchIPFSStatus();
      },10*1000)    
    } catch(e) {
      if(typeof self.onError === 'function') {
        self.onError(err);
      }
    }

    this.node.id((err, identity) => {
      if (err) {
        console.log('_watchIPFSStatus err=<',err,'>');
        if(typeof self.onError === 'function') {
          self.onError(err);
        }
      }
    });
    
  }
  _restartIpfs() {
    try {
      execSync('docker stack rm go-crystal');
      execSync('sleep 60');
      execSync('cd /ceph/storage3/ipfs/crystal.wator/go-ipfs-cluster && docker stack deploy go-crystal -c docker-compose.yml');
      console.log('_restartIpfs');
    } catch(e) {
      console.log('_restartIpfs e=<',e,'>');
    }
  }
}
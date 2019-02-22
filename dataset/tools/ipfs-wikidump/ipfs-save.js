const ipfsAPI = require('ipfs-http-client');
const strConstAddress = '/ip4/127.0.0.1/tcp/5001';
const iConstSavePageOneTime = 2000;

module.exports = class IpfsSave {
  constructor() {
    this._tryOpenIpfsNode();
  }
  save(cnTitle,cnText,pos,cb) {
    let bufText = Buffer.from(cnText, 'utf8');
    ipfs.add(bufText,(err, result) =>{
      if (err) {
        console.log('save2Ipfs::err=<',err,'>');
        return;
      }
      //console.log('save2Ipfs::result=<',result,'>');
      let hash = result[0].hash;
      //console.log('save2Ipfs::hash=<',hash,'>');
      //console.log('save2Ipfs::cnTitle=<',cnTitle,'>');
      //console.log('save2Ipfs::pos=<',pos,'>');
      if(hash && typeof cb === 'function') {
        cb(hash);
      }
    });
  }
  _tryOpenIpfsNode () {
    this.node = ipfsAPI(strConstAddress);
    this.iSavePageCounter = 0;
    let self = this;
    this.node.id((err, identity) => {
      if (err) {
        setTimeout(()=>{
          self._tryOpenIpfsNode();
        },10*1000);
        return;
      }
      console.log('_tryOpenIpfsNode::this.node=<',this.node,'>');
      console.log('_tryOpenIpfsNode identity=<',identity,'>');
    });    
  }
}
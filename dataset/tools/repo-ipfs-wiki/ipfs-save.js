const IPFS = require('ipfs');
const config = { 
  repo: './ipfs_repos_public',
  config: {
    Addresses: {
      Swarm: [
        '/ip6/::/tcp/4006'
      ]
    },
    relay: {
      enabled: true, hop: { enabled: true, active: true }
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
      '/ip4/104.236.176.52/tcp/4001/ipfs/QmSoLnSGccFuZQJzRadHn95W2CrSFmZuTdDWP8HXaHca9z',
      '/ip4/104.131.131.82/tcp/4001/ipfs/QmaCpDMGvV2BGHeYERUEnRQAwe3N8SzbUtfsmvsqQLuvuJ',
      '/ip4/104.236.179.241/tcp/4001/ipfs/QmSoLPppuBtQSGwKDZT2M73ULpjvfd3aZ6ha4oFGL1KrGM',
      '/ip4/162.243.248.213/tcp/4001/ipfs/QmSoLueR4xBeUbY9WZ9xGUUxunbKWcrNFTDAadQJmocnWm',
      '/ip4/128.199.219.111/tcp/4001/ipfs/QmSoLSafTMBsPKadTEgaXctDQVcqN88CNLHXMkTNwMKPnu',
      '/ip4/104.236.76.40/tcp/4001/ipfs/QmSoLV4Bbm51jM9C4gDYZQ9Cy3U6aXMJDAbzgu2fzaDs64',
      '/ip4/178.62.158.247/tcp/4001/ipfs/QmSoLer265NRgSp2LA3dPaeykiS1J6DifTC88f5uVQKNAd',
      '/ip4/178.62.61.185/tcp/4001/ipfs/QmSoLMeWqB7YGVLJN3pNLQpmmEk35v6wYtsMGLzSr5QBU3',
      '/ip4/104.236.151.122/tcp/4001/ipfs/QmSoLju6m7xTh3DuokvT3886QRYqxAzb1kShaanJgW36yx',
      '/ip6/2604:a880:1:20::1f9:9001/tcp/4001/ipfs/QmSoLnSGccFuZQJzRadHn95W2CrSFmZuTdDWP8HXaHca9z',
      '/ip6/2604:a880:1:20::203:d001/tcp/4001/ipfs/QmSoLPppuBtQSGwKDZT2M73ULpjvfd3aZ6ha4oFGL1KrGM',
      '/ip6/2604:a880:0:1010::23:d001/tcp/4001/ipfs/QmSoLueR4xBeUbY9WZ9xGUUxunbKWcrNFTDAadQJmocnWm',
      '/ip6/2400:6180:0:d0::151:6001/tcp/4001/ipfs/QmSoLSafTMBsPKadTEgaXctDQVcqN88CNLHXMkTNwMKPnu',
      '/ip6/2604:a880:800:10::4a:5001/tcp/4001/ipfs/QmSoLV4Bbm51jM9C4gDYZQ9Cy3U6aXMJDAbzgu2fzaDs64',
      '/ip6/2a03:b0c0:0:1010::23:1001/tcp/4001/ipfs/QmSoLer265NRgSp2LA3dPaeykiS1J6DifTC88f5uVQKNAd',
      '/ip6/2a03:b0c0:1:d0::e7:1/tcp/4001/ipfs/QmSoLMeWqB7YGVLJN3pNLQpmmEk35v6wYtsMGLzSr5QBU3',
      '/ip6/2604:a880:1:20::1d9:6001/tcp/4001/ipfs/QmSoLju6m7xTh3DuokvT3886QRYqxAzb1kShaanJgW36yx',
      '/dns4/node0.preload.ipfs.io/tcp/443/wss/ipfs/QmZMxNdpMkewiVZLMRxaNxUeZpDUb34pWjZ1kZvsd16Zic',
      '/dns4/node1.preload.ipfs.io/tcp/443/wss/ipfs/Qmbut9Ywz9YEDrz8ySBSgWyJk41Uvm2QJPhwDJzJyGFsD6'
    ],
    Swarm: {
      ConnMgr: {
        LowWater: 600,
        HighWater: 900
      }
    }
  }
};
/*
const node = new IPFS(config);
*/
const execSync= require('child_process').execSync;

module.exports = class IpfsSave {
  constructor() {
    this._tryOpenIpfsNode();
    this.cacheToSave = [];
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
  }
  _restartIpfs() {
  }
}

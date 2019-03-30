const IPFS_GATEWAY = 'https://ipfs.wator.xyz/ipfs';
const CRYSTAL_WS_POOL = 'wss://crystal.wator.xyz/crystal/player/wss';

class WaiCrystal {
  constructor(wai) {
    this.block_ = 'QmcbsjA8dzaGVXJ1PbGsKoZfZWKvSecRb3Kty996rQXZkQ';
    this.blockURL_ = IPFS_GATEWAY + '/' + this.block_;
    this.idle_ = true;
    this.wai_ = wai;
    let self = this;
    // debug
    /*
    setTimeout(()=>{
      //$.get(self.blockURL_,self.onBlockFromIpfs_.bind(self));
    },1);
    */
    this.ws_ = new WebSocket(CRYSTAL_WS_POOL);
    this.ws_.onopen = this.onPoolOpened_.bind(this);
    this.ws_.onerror = this.onPoolError_.bind(this);
    this.ws_.onclose = this.onPoolClosed_.bind(this);
    this.ws_.onmessage = this.onPoolMessage_.bind(this);
  }
  async onBlockFromIpfs_(block) {
    let startTP = new Date();
    this.idle_ = false;
    if(typeof this.entryBlock === 'function') {
      this.entryBlock();
    }
    try {
      let jsonBlock = JSON.parse(block);
      //console.log('WaiCrystal::onBlockFromIpfs_ jsonBlock=<',jsonBlock,'>');
      for(let resourceCID of jsonBlock.resource) {
        //console.log('WaiCrystal::onBlockFromIpfs_ resourceCID=<',resourceCID,'>');
        let rURL = IPFS_GATEWAY + '/' + resourceCID;
        let result = await $.get(rURL);
        //console.log('WaiCrystal::onBlockFromIpfs_ result=<',result,'>');
        let articleStart = new Date();
        this.onArticle(result);
        let escapeArticle = new Date() - articleStart;
        console.log('WaiCrystal::onBlockFromIpfs_ escapeArticle =<',escapeArticle,'>ms');
      }
      console.log('WaiCrystal::onBlockFromIpfs_ this.currentTaskCID_=<',this.currentTaskCID_,'>');
      let crystalCollect = this.leaveBlock();
      console.log('WaiCrystal::onBlockFromIpfs_ this.crystalCollect=<',this.crystalCollect,'>');
    }
    catch(e) {
      console.error('WaiCrystal::onBlockFromIpfs_ e=<',e,'>');
    }
    let now = new Date();
    let escape = now - startTP;
    console.log('WaiCrystal::onBlockFromIpfs_ escape/1000 =<',escape/1000,'>seconds');
    this.idle_ = true;
  }
  onPoolOpened_(evt) {
    console.log('WaiCrystal::onPoolOpened_ evt=<',evt,'>');
  }
  onPoolError_(evt) {
    console.log('WaiCrystal::onPoolError_ evt=<',evt,'>');
  }
  onPoolClosed_(evt) {
    console.log('WaiCrystal::onPoolClosed_ evt=<',evt,'>');
  }
  onPoolMessage_(evt) {
    //console.log('WaiCrystal::onPoolMessage_ evt.data=<',evt.data,'>');
    try {
      let msgJson = JSON.parse(evt.data);
      //console.log('WaiCrystal::onPoolMessage_ msgJson=<',msgJson,'>');
      if(msgJson && msgJson.cid) {
        this.onBlockTask_(msgJson);
      }
    } catch (e) {
      console.log('WaiCrystal::onPoolMessage_ e=<',e,'>');
    }
  }
  onBlockTask_(task) {
    //console.log('WaiCrystal::onBlockTask_ task=<',task,'>');
    //console.log('WaiCrystal::onBlockTask_ this.idle_=<',this.idle_,'>');
    if(this.idle_) {
      let url = IPFS_GATEWAY + '/' + task.cid;
      this.currentTaskCID_ = task.cid;
      $.get(url,this.onBlockFromIpfs_.bind(this));
      let resStartTask = JSON.parse(JSON.stringify(task));
      resStartTask.stage = 'start';
      this.ws_.send(JSON.stringify(resStartTask));
    }
  }
}

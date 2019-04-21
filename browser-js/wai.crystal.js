const CRYSTAL_GATEWAY = 'https://crystal.wator.xyz:8443/crystal';
const CRYSTAL_WS_POOL = 'wss://crystal.wator.xyz/crystal/player/wss';

class WaiCrystal {
  constructor(wai) {
    this.block_ = 'QmcbsjA8dzaGVXJ1PbGsKoZfZWKvSecRb3Kty996rQXZkQ';
    this.blockURL_ = CRYSTAL_GATEWAY + '/' + this.block_;
    this.idle_ = true;
    this.wai_ = wai;
    let self = this;
    // debug
    /*
    setTimeout(()=>{
      //$.get(self.blockURL_,self.onBlockFromCrystalPool_.bind(self));
    },1);
    */
    this.ws_ = new WebSocket(CRYSTAL_WS_POOL);
    this.ws_.onopen = this.onPoolOpened_.bind(this);
    this.ws_.onerror = this.onPoolError_.bind(this);
    this.ws_.onclose = this.onPoolClosed_.bind(this);
    this.ws_.onmessage = this.onPoolMessage_.bind(this);
  }
  async onBlockFromCrystalPool_(block) {
    let startTP = new Date();
    this.idle_ = false;
    this.error_ = false;
    this.wai_.entryBlock();
    try {
      let jsonBlock = JSON.parse(block);
      //console.log('WaiCrystal::onBlockFromCrystalPool_ jsonBlock=<',jsonBlock,'>');
      for(let resourceAddress of jsonBlock.resource) {
        //console.log('WaiCrystal::onBlockFromCrystalPool_ resourceAddress=<',resourceAddress,'>');
        let rURL = CRYSTAL_GATEWAY + '/' + resourceAddress;
        let result = await $.get(rURL);
        let address = this.addressOfContent_(result);
        //console.log('WaiCrystal::onBlockFromCrystalPool_ result=<',result,'>');
        if(address !== resourceAddress) {
          this.error_ = true;
          break;
        }
        let articleStart = new Date();
        this.wai_.article(result);
        let escapeArticle = new Date() - articleStart;
        //console.log('WaiCrystal::onBlockFromCrystalPool_ escapeArticle =<',escapeArticle,'>ms');
      }
      console.log('WaiCrystal::onBlockFromCrystalPool_ this.currentTaskCID_=<',this.currentTaskCID_,'>');
      
      console.log('WaiCrystal::onBlockFromCrystalPool_ this.error_=<',this.error_,'>');
      let wordCollect = this.wai_.leaveBlock();
      console.log('WaiCrystal::onBlockFromCrystalPool_ wordCollect=<',wordCollect,'>');
      let now = new Date();
      let escape = now - startTP;
      console.log('WaiCrystal::onBlockFromCrystalPool_ escape/1000 =<',escape/1000,'>seconds');
      this.currentTask_.stage = 'finnish';
      this.currentTask_.error = this.error_;
      this.currentTask_.words = wordCollect;
      this.ws_.send(JSON.stringify(this.currentTask_));
    }
    catch(e) {
      console.error('WaiCrystal::onBlockFromCrystalPool_ e=<',e,'>');
    }
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
      let url = CRYSTAL_GATEWAY + '/' + task.cid;
      this.currentTaskCID_ = task.cid;
      $.get(url,this.onBlockFromCrystalPool_.bind(this));
      let resStartTask = JSON.parse(JSON.stringify(task));
      this.currentTask_ = Object.assign({}, task);;
      resStartTask.stage = 'start';
      this.ws_.send(JSON.stringify(resStartTask));
    }
  }
  addressOfContent_ = (content) => {
    let shake = shake256(content,160);
    //console.log('addressOfContent shake=<',shake,'>');
    const bytes = this.fromHexString_(shake);
    //console.log('addressOfContent bytes=<',bytes,'>');  
    const address = Base58.encode(bytes);
    //console.log('addressOfContent address=<',address,'>');  
    return address;
  }
  fromHexString_(hexString) {
    return new Uint8Array(hexString.match(/.{1,2}/g).map(byte => parseInt(byte, 16)));
  }
}

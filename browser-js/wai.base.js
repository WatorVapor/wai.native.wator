const iConstWordFilterOutStageOne = 3;

class WaiBase {
  constructor() {
    
  }
  entryBlock() {
    this.collectBlock_ = {};
    //console.log('WaiBase::entryBlock this.collectBlock_=<',this.collectBlock_,'>');
    this.start_ = new Date();
  }
  leaveBlock() {
    //console.log('WaiBase::leaveBlock this.collectBlock_=<',this.collectBlock_,'>');
    let collectOfBlockStr = JSON.stringify(this.collectBlock_,undefined,2);
    console.log('WaiBase::leaveBlock collectOfBlockStr=<',collectOfBlockStr,'>');
    let now = new Date();
    let escape = now - this.start_;
    console.log('WaiBase::leaveBlock escape/1000 =<',escape/1000,'>seconds');
  }
  
  
  article(doc,onSentence) {
    let aDocumentStatistics = {};
    //console.log('article doc=<',doc,'>');
    let cjkCollect = [];
    let cjkBuffer = [];
    for(let i = 0 ;i < doc.length;i++) {
      let utf8 = doc[i];
      //console.log('article utf8=<',utf8,'>');
      let isCJK = CJK_Table[utf8];
      //console.log('article isCJK=<',isCJK,'>');
      if(isCJK) {
        cjkBuffer.push(utf8);
      } else {
        if(cjkBuffer.length > 0) {
          cjkCollect.push(cjkBuffer);
        }
        cjkBuffer = [];
      }
    }
    if(cjkBuffer.length > 0) {
      cjkCollect.push(cjkBuffer);
    }  
    //console.log('article cjkCollect=<',cjkCollect,'>');
    for(let i = 0 ;i < cjkCollect.length;i++) {
      onSentence(cjkCollect[i],aDocumentStatistics);
    }
    //console.log('article aDocumentStatistics=<',aDocumentStatistics,'>');
    let highFreq = this.FilterOutLowFreq_(aDocumentStatistics);
    //console.log('article highFreq=<',highFreq,'>');
    let uniqWords = this.FilterOutInside_(highFreq);
    //console.log('article uniqWords=<',uniqWords,'>');
    this.mergeCollect_(uniqWords);
    //return uniqWords;    
  }

  // inside
  mergeCollect_ = (collect) => {
    //console.log('mergeCollect_ collect=<',collect,'>');
    for(let key in collect) {
      if(this.collectBlock_[key]) {
        this.collectBlock_[key] += collect[key];
      } else {
        this.collectBlock_[key] = collect[key];
      }
    }
  }

  FilterOutLowFreq_ = (collect) => {
    let outCollect = JSON.parse(JSON.stringify(collect));
    let keys = Object.keys(outCollect);
    for(let i = 0 ;i < keys.length;i++) {
      let key = keys[i];
      if(outCollect[key] < iConstWordFilterOutStageOne) {
        delete outCollect[key];
      }
    }
    return outCollect;
  }

  FilterOutInside_ = (collect) => {
    let outCollect = JSON.parse(JSON.stringify(collect));
    let keys = Object.keys(outCollect);
    for(let i = 0 ;i < keys.length;i++) {
      let key = keys[i];
      //console.log('FilterOutInside_ key=<',key,'>');
      for(let j = 0 ;j < keys.length;j++) {
        let keyFound = keys[j];
        //console.log('FilterOutInside_ keyFound=<',keyFound,'>');
        if(keyFound !== key && keyFound.includes(key)) {
          if(outCollect[keyFound] === outCollect[key]) {
            delete outCollect[key];
          }
        }
      }
    }
    return outCollect;
  }

}

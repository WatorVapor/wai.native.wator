const request = require('request');
const cheerio = require('cheerio');
const wordDB = require('./wordFromLevelDB.js')


module.exports = class WikiDict {
  constructor(option) {
    if(option.root) {
      this.root = option.root;
    }
    this.dry = false;
    if(option.dry) {
      this.dry = option.dry;
    }
    this.db = {
      input:'../../input_db/zhizi/ja',
      output:'../../out_db/ja',
    };
    this.dictDB = new wordDB(this.db);
  }
    
  runOnce(cb){
    this.cb = cb;
    this.dictDB.getWord(function(word){
      console.log('runOnce::word=<',word,'>');
    });
    this.word = '解決';
    let url = this.root + encodeURIComponent(this.word);
    this.getOneTitle_(url);
  }
  
  readKana_() {
    let kana1 = this.plainText.split('）');
    //console.log('readKana_::kana1=<',kana1,'>');
    if(kana1.length > 0) {
      let kana2 = kana1[0].split('（');
      //console.log('readKana_::kana2=<',kana2,'>');
      if(kana2.length > 1) {
        //console.log('readKana_::kana2[1]=<',kana2[1],'>');
        let kana = kana2[1];
        console.log('readKana_::this.word=<',this.word,'>','kana=<',kana,'>');
      }
    }
  }
  
  

  getOneTitle_(url) {
    let options = {
      url:url,
      encoding: null
    };
    let self = this;
    console.log('getOneTitle_:: options=<',options,'>');
    request.get(options, function (error, response, body) {
      if (error) {
        console.log('error: url=<',url,'>');
        console.log('error: error=<',error,'>');
        console.log('error: typeof error=<',typeof error,'>');
        console.log('error: error.code=<',error.code,'>');
        console.log('error: error.message=<',error.message,'>');
        console.log('error: response=<',response,'>');
        this.cb();
      } else {
        //console.log('response=<',response,'>');
        //console.log('body=<',body,'>');
        if(response && response.statusCode === 200) {
          self.parseHTML_(body,url);
        } else {
          this.cb();
        }
      }
    });
  }
  
  
  parseHTML_(data,url) {
    this.plainText = '';
    this.hrefsLinks = [];
    let self = this;
    try {
      //console.log('data=<',data,'>');
      const $ = cheerio.load(data);
      $('p').each( (i, elem) => {
        //console.log('i=<',i,'>');
        //console.log('elem=<',elem,'>');
        elem.children.forEach( (value, index, ar) => {
          //console.log('value=<',value,'>');
          self.getTextAllChildren_(value);
        });
        this.plainText += '';
      });
    } catch(e) {
      console.log('e=<',e,'>');
    }
    console.log('this.plainText=<',this.plainText,'>');
    this.readKana_();
  }
  
  getTextAllChildren_(elem){
    if(elem.type === 'text') {
      if(this.dry) {
        //console.log('elem=<',elem,'>');
        console.log('elem.data=<',elem.data,'>');
        console.log('typeof elem.data=<',typeof elem.data,'>');
        console.log('elem.data.length=<',elem.data.length,'>');
      }
      let length = elem.data.length;
      let textPure = elem.data.substr(0,length);
      this.plainText += textPure;
    }
    if(elem.children && Array.isArray(elem.children)) {
      let self = this;
      //console.log('elem.children=<',elem.children,'>');
      elem.children.forEach( (valueA, indexA, arA) => {
        self.getTextAllChildren_(valueA);
      });
    }
  }
}

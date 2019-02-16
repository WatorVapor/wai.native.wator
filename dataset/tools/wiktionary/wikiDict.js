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
      input:'../../input_db/zhizi/cn',
      output:'../../out_db/cn',
      done:'../../out_db/cn_done'
    };
    this.dictDB = new wordDB(this.db);
  }
    
  runOnce(cb){
    this.cb = cb;
    let once = this.dictDB.getWord(function(word){
      //console.log('runOnce::word=<',word,'>');
      this.word = word;
      //this.word = '学习';
      //this.word = '三洋';
      //this.word = '三田';
      //this.word = '西安';
      if(this.isAllHanzi(word)) {
        let url = this.root + encodeURIComponent(this.word);
        console.log('runOnce::url=<',url,'>');
        this.getOneTitle_(url);
      } else {
        console.log('runOnce::word=<',word,'>');
        this.cb();
      }
    }.bind(this));
    //console.log('runOnce::once=<',once,'>');
    if(once ==='prepare') {
      setTimeout(function(){
        this.cb();
      }.bind(this),5000);
    }
  }
  
  

  getOneTitle_(url) {
    let options = {
      url:url,
      encoding: null
    };
    let self = this;
    //console.log('getOneTitle_:: options=<',options,'>');
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
          self.cb();
        }
      }
    });
  }
  
  
  parseHTML_(data,url) {
    this.plainText = '';
    this.hrefsLinks = [];
    let self = this;
    this.hintPinYin = false;
    try {
      //console.log('data=<',data,'>');
      const $ = cheerio.load(data);
      $('tt').each( (i, elem) => {
        //console.log('i=<',i,'>');
        //console.log('elem=<',elem,'>');
        elem.children.forEach( (value, index, ar) => {
          //console.log('value=<',value,'>');
          self.getTextAllChildren_(value);
        });
        this.plainText += ',';
      });
      if(this.plainText) {
        console.log('this.word=<',this.word,'>','this.plainText=<',this.plainText,'>');
        this.dictDB.setWordPinYin(this.word,this.plainText);
        this.cb();
        return;
      }
      
      
      $('td').each( (i, elem) => {
        //console.log('i=<',i,'>');
        //console.log('elem=<',elem,'>');
        elem.children.forEach( (value, index, ar) => {
          //console.log('value=<',value,'>');
          self.getTextAllChildren_(value);
        });
        this.plainText += ',';
      });
      $('li').each( (i, elem) => {
        //console.log('i=<',i,'>');
        //console.log('elem=<',elem,'>');
        elem.children.forEach( (value, index, ar) => {
          //console.log('value=<',value,'>');
          self.getTextAllChildren_(value);
        });
        this.plainText += ',';
      });
    } catch(e) {
      console.log('e=<',e,'>');
    }
    if(this.plainText) {
      //console.log('this.word=<',this.word,'>','this.plainText=<',this.plainText,'>');
      self.getPinYin(this.plainText);
    }
    this.cb();
  }
  
  getPinYin(text) {
    //console.log('getPinYin::text=<',text,'>');
    let pinYin1 = text.split('汉语拼音');
    //console.log('getPinYin::pinYin1[0]=<',pinYin1[0],'>');
    if(pinYin1.length >1) {
      let pinYin2 = pinYin1[1].split(',');
      let pinYin = pinYin2[0].replace('：','').replace(':','').trim();
      console.log('pinYin2::pinYin=<',pinYin,'>');
      this.dictDB.setWordPinYin(this.word,pinYin);
    }
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
      //console.log('textPure=<',textPure,'>');
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

  includeHanzi(utf8Str) {
    //console.log('includeHanzi::utf8Str=<',utf8Str,'>');
    for(let i = 0;i < utf8Str.length;i++) {
      //console.log('includeHanzi::typeof utf8Str[i]=<',typeof utf8Str[i],'>');
      //console.log('includeHanzi::utf8Str[i]=<',utf8Str[i],'>');
      let hanzi = utf8Str[i];
      for(let j = 0;j < HanziRange.length;j++) {
        if(hanzi >= HanziRange[j].b && hanzi <= HanziRange[j].e) {
          return true;
        }
      }
    }
    return false;
  }

  isAllHanzi(utf8Str) {
    //console.log('includeHanzi::utf8Str=<',utf8Str,'>');
    for(let i = 0;i < utf8Str.length;i++) {
      //console.log('includeHanzi::typeof utf8Str[i]=<',typeof utf8Str[i],'>');
      //console.log('includeHanzi::utf8Str[i]=<',utf8Str[i],'>');
      let hanzi = utf8Str[i];
      let isHanzi = false;
      for(let j = 0;j < HanziRange.length;j++) {
        if(hanzi >= HanziRange[j].b && hanzi <= HanziRange[j].e) {
          isHanzi = true;
          break;
        }
      }
      if(!isHanzi) {
        return false;
      }
    }
    return true;
  }

  
}

const HanziRange = [ 
  {b:'㐀',e:'䶵'},
  {b:'一',e:'龥'},
  {b:'龦',e:'鿋'},
  {b:'𠀀',e:'𪛖'},
  {b:'𪜀',e:'𫜴'},
  {b:'𫝀',e:'𫠝'},

  {b:'⼀',e:'⿕'},
  {b:'⺀',e:'⻳'},
  {b:'豈',e:'龎'},
  {b:'丽',e:'鼻'},/* +1 */

  {b:'㇀',e:'㇣'},
  {b:'⿰',e:'⿻'},
  {b:'ㄅ',e:'ㄠ'},
  {b:'ㆠ',e:'ㆷ'},

  {b:'〇',e:'〇'},
  
];


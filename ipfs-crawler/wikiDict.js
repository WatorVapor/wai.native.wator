var request = require('request');
const cheerio = require('cheerio');
var redis = require('redis');
var crypto = require("crypto");
var ipfsAPI = require('ipfs-api');


const redisOption = {
  port:6379,
  host:'master.redis.wator.xyz',
  family:'IPv6'
};

const redisKeyPrefix = '/wator/wai/crawler/wiki';
const redisKeyPrefixDone = '/wator/wai/crawler/wiki/done';
const redisKeyPrefixTodo = '/wator/wai/crawler/wiki/todo';
const redisKeyPrefixIpfs = '/wator/wai/wiki/text';

const filterWiki = [
  '/Wikipedia:','/Help:','/Portal:',
  '/Special:', '/Project:','/Talk:',
  'Template:','/File:','/Template_talk:',
  '/User:'
];


var ipfs = ipfsAPI({host: 'localhost', port: '5001', protocol: 'http'});

module.exports = class WikiCrawler {
  constructor(option) {
    if(option.prefix) {
      this.prefix = option.prefix;
    }
    if(option.replace) {
      this.replace = option.replace;
    }
    if(option.root) {
      this.root = option.root;
    }
    if(option.seed) {
      this.seed = option.seed;
    }
    if(option.tag) {
      this.tag = option.tag;
    }
    this.dry = false;
    if(option.dry) {
      this.dry = option.dry;
    }
    this.cursor = '0';
    this.client = redis.createClient(redisOption);
    this.first = true;
    this.startTP = new Date();
    this.wikiDoneCounter = 0;
    //console.log('WikiCrawler::constructor this=<',this,'>');
  }
  
  
  runOnce(cb){
    this.cb = cb;
    this.ipfsWritten = false;
    this.todoWritten = false;
    this.doneWritten = false;
    this.runTopTodo_();    
  }
  
  runTopTodo_() {
    let self = this;
    //console.log('this.cursor=<',this.cursor,'>');
    this.client.scan(this.cursor,
                     'MATCH',redisKeyPrefixTodo + '/*',
                     'COUNT', '1',
                     function (err, res) {
      if (err) {
        console.log('err=<',err,'>');
        self.onApiError_();
      }
      //console.log('res=<',res,'>');
      self.cursor = res[0];
      let keys = res[1];
      //console.log('keys=<',keys,'>');
      if(keys.length > 0) {
        //console.log('keys[0]=<',keys[0],'>');
        self.client.get(keys[0], function (err, wikiUrl) {
          if (err) {
            console.log('err=<',err,'>');
            self.onApiError_();
          }
          console.log('wikiUrl=<',wikiUrl,'>');
          self.getOneTitle_(wikiUrl);
          let escape = new Date() - self.startTP;
          self.wikiDoneCounter++;
          //console.log('self.wikiDoneCounter=<',self.wikiDoneCounter,'>');
          //console.log('escape=<',escape,'>');
          let doneWikiPerMinute  = 60*1000* self.wikiDoneCounter / escape;
          console.log('doneWikiPerMinute=<',doneWikiPerMinute,'>');
        });
      } else {
        //console.log('self.first=<',self.first,'>');
        if(self.first) {
          let wikiUrl = self.root + self.seed;
          console.log('wikiUrl=<',wikiUrl,'>');
          self.getOneTitle_(wikiUrl);
          self.first = false;
        } else {
          if(typeof self.cb === 'function') {
            self.cb();
          }
        }
      }
    });
  }
  

  getOneTitle_(url) {
    let options = {
      url:url,
      encoding: null
    };
    let self = this;
    request.get(options, function (error, response, body) {
      if (error) {
        console.log('error: url=<',url,'>');
        console.log('error: error=<',error,'>');
        console.log('error: typeof error=<',typeof error,'>');
        console.log('error: error.code=<',error.code,'>');
        console.log('error: error.message=<',error.message,'>');
        console.log('error: response=<',response,'>');
        if(error.code === 'ERR_INVALID_CHAR') {
          self.todoWritten = true;
          self.saveDoneWiki_(url,'');
        } else {
          self.onApiError_();
        }
      } else {
        //console.log('response=<',response,'>');
        //console.log('body=<',body,'>');
        if(response && response.statusCode === 200) {
          self.parseHTML_(body,url);
        } else {
          self.todoWritten = true;
          self.saveDoneWiki_(url,'');
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
        this.plainText += '\n';
      });
      $('*').each( (i, elem) => {
        //console.log('i=<',i,'>');
        //console.log('elem=<',elem,'>');
        self.getURLAllChildren_(elem);
      });
    } catch(e) {
      console.log('e=<',e,'>');
    }
    if(!this.dry) {
      this.saveDoneWiki_(url,this.plainText);
      this.saveLinkedWiki_(this.hrefsLinks);
    } else {
      console.log('this.plainText=<',this.plainText,'>');
      let uniqueHrefsLinks = this.hrefsLinks.filter((animal, index, array) => {
        return array.indexOf(animal) === index;
      });
      console.log('uniqueHrefsLinks=<',uniqueHrefsLinks.join(',\n'),'>');
    }
    //console.log('this.plainText=<',this.plainText,'>');
    //console.log('this.hrefsLinks=<',this.hrefsLinks,'>');
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
  
  getURLAllChildren_(elem){
    let self = this;
    if(elem.type === 'tag' && elem.name === 'a') {
      if(elem.attribs.href && elem.attribs.href.startsWith(this.prefix) && typeof elem.attribs.title === 'string') {
        //console.log('typeof elem.attribs.title=<',typeof elem.attribs.title,'>');
        let filterout = false;
        for(let i = 0;i<filterWiki.length;i++) {
          let filter = filterWiki[i];
          if(elem.attribs.href.indexOf(filter) !== -1) {
            //console.log('elem.attribs.href=<',elem.attribs.href,'>');
            filterout = true;
            continue;
          }
        }
        if(filterout == false) {
          if(this.replace) {
            let newHref = elem.attribs.href.replace(this.prefix,this.replace);
            //console.log('newHref=<',newHref,'>');
            this.hrefsLinks.push(this.root + newHref);
          } else {
            this.hrefsLinks.push(this.root + value.attribs.href);
          }
        }
      }
    }
    elem.children.forEach( (value, index, ar) => {
      if(value.type === 'tag' && value.name === 'a') {
        self.getURLAllChildren_(value);
      }
    });
  }

  
  saveDoneWiki_(url,plainText){
    let hashURL = this.sha512_(url);
    //console.log('url=<',url,'>');
    //console.log('hashURL=<',hashURL,'>');
    this.client.set(redisKeyPrefixDone + '/' + hashURL, url);
    this.client.del(redisKeyPrefixTodo + '/' + hashURL, url);
    this.doneWritten = true
    let bufText = Buffer.from(plainText, 'utf8');
    let self = this;
    
    ipfs.files.add(bufText,function(err, result) {
      if (err) {
        console.log('err=<',err,'>');
        self.onApiError_();
        return;
      }
      try {
        if(result.length > 0){
          console.log('result=<',result,'>');
          let ipfsPath =result[0].hash;
          console.log('ipfsPath=<',ipfsPath,'>');
          let saveInfo ={url:url};
          self.client.set(redisKeyPrefixIpfs + '/' + self.tag + '/ipfs/'+ ipfsPath, JSON.stringify(saveInfo));
          self.ipfsWritten = true;
          console.log('self.ipfsWritten=<',self.ipfsWritten,'>');
          console.log('self.todoWritten=<',self.todoWritten,'>');
          console.log('self.doneWritten=<',self.doneWritten,'>');
          if(self.ipfsWritten && self.todoWritten && self.doneWritten) {
            self.cb();
          }
        }
      } catch(e) {
        console.log('e=<',e,'>');
        self.onApiError_();
      }
    });
    
  }
  
  
  
  saveLinkedWiki_(hrefsLinks){
    //console.log('hrefsLinks.length=<',hrefsLinks.length,'>');
    let uniqueHrefsLinks = hrefsLinks.filter((animal, index, array) => {
      return array.indexOf(animal) === index;
    });
    
    let counter = uniqueHrefsLinks.length;
    //console.log('counter=<',counter,'>');
    for(let i = 0;i < uniqueHrefsLinks.length ;i++) {
      let link = uniqueHrefsLinks[i];
      //console.log('link=<',link,'>');
      let hashLink = this.sha512_(link);
      //console.log('hashLink=<',hashLink,'>');
      let self = this;
      this.client.get(redisKeyPrefixDone + '/' + hashLink, function (err, resultInDone) {
        counter -= 1;
        if (err) {
          console.log('err=<',err,'>');
          self.onApiError_();
          return;
        }
        //console.log('resultInDone=<',resultInDone,'>');
        if(!resultInDone) {
          self.client.get(redisKeyPrefixTodo + '/' + hashLink, function (err, resultTodo) {
            if (err) {
              console.log('err=<',err,'>');
              self.onApiError_();
              return;
            }
            //console.log('resultTodo=<',resultTodo,'>');
            if(!resultTodo) {
              self.client.set(redisKeyPrefixTodo + '/' + hashLink, link);
            }
          });
        }
        //console.log('counter=<',counter,'>');
        if(counter === 0) {
          self.todoWritten = true;
          console.log('self.ipfsWritten=<',self.ipfsWritten,'>');
          console.log('self.todoWritten=<',self.todoWritten,'>');
          console.log('self.doneWritten=<',self.doneWritten,'>');
          if(self.ipfsWritten && self.todoWritten && self.doneWritten) {
            self.cb();
          }
        }
      });
    }
    if(counter < 1) {
      console.log('counter=<',counter,'>');
      this.todoWritten = true;
      console.log('this.ipfsWritten=<',this.ipfsWritten,'>');
      console.log('this.todoWritten=<',this.todoWritten,'>');
      console.log('this.doneWritten=<',this.doneWritten,'>');
      if(this.ipfsWritten && this.todoWritten && this.doneWritten) {
        this.cb();
      }
    }
  }
  
  sha512_(msg) {
    let sha512 = crypto.createHash('sha512');
    sha512.update(msg);
    let hash = sha512.digest('hex');
    return hash;
  }
  onApiError_() {
    process.exit(0);
  }
}

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
    this.client = redis.createClient(redisOption);
    //console.log('WikiCrawler::constructor this=<',this,'>');
  }
  
  
  runOnce(cb){
    this.cb = cb;
    this.ipfsWritten = false;
    this.todoWritten = false;
    this.doneWritten = false;
    this.runTopTodo_();
  }
  
  /*
  runTopTodo_() {
    let self = this;
    this.client.keys(redisKeyPrefixTodo + '/*', function (err, keys) {
      if (err) {
        console.log('err=<',err,'>');
        self.onApiError_();
      }
      if(keys.length > 0) {
        console.log('keys[0]=<',keys[0],'>');
        self.client.get(keys[0], function (err, wikiUrl) {
        if (err) {
          console.log('err=<',err,'>');
          self.onApiError_();
        }
        console.log('wikiUrl=<',wikiUrl,'>');
          self.getOneTitle_(wikiUrl);
        });
      } else {
        self.getOneTitle_(self.root + self.seed);
      }
    });
  }
  */

  runTopTodo_() {
    let self = this;
    let cursor = '0';
    this.client.scan(cursor,
                     'MATCH',redisKeyPrefixTodo + '/*',
                     'COUNT', '10',
                     function (err, res) {
      if (err) {
        console.log('err=<',err,'>');
        self.onApiError_();
      }
      console.log('res=<',res[0],'>');
      let keys = res[1];
      console.log('keys=<',keys,'>');
      if(keys.length > 0) {
        console.log('keys[0]=<',keys[0],'>');
        self.client.get(keys[0], function (err, wikiUrl) {
        if (err) {
          console.log('err=<',err,'>');
          self.onApiError_();
        }
        console.log('wikiUrl=<',wikiUrl,'>');
          self.getOneTitle_(wikiUrl);
        });
      } else {
        self.getOneTitle_(self.root + self.seed);
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
        console.log('error: error=<',error,'>');
        console.log('error: response=<',response,'>');
        if(error) {
          self.onApiError_();
        }
      } else {
        //console.log('body=<',body,'>');
        if(response && response.statusCode === 200) {
          self.parseHTML_(body,url);
        }
      }
    });
  }
  
  
  parseHTML_(data,url) {
    this.plainText = '';
    let hrefsLinks = [];
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
          if(value.type === 'tag' && value.name === 'a') {
            //console.log('value=<',value,'>');
            if(value.attribs.href && value.attribs.href.startsWith(this.prefix)) {
              if(this.replace) {
                let newHref = value.attribs.href.replace(this.prefix,this.replace);
                //console.log('newHref=<',newHref,'>');
                hrefsLinks.push(this.root + newHref);
              } else {
                hrefsLinks.push(this.root + value.attribs.href);
              }
            } else {
              //console.log('value.attribs.href=<',value.attribs.href,'>');
            }
          }
          //console.log('index=<',index,'>');
        });
        this.plainText += '\n';
      });
    } catch(e) {
      console.log('e=<',e,'>');
    }
    if(!this.dry) {
      this.saveDoneWiki_(url,this.plainText);
      this.saveLinkedWiki_(hrefsLinks);
    } else {
      console.log('this.plainText=<',this.plainText,'>');
    }
    //console.log('this.plainText=<',this.plainText,'>');
    //console.log('hrefsLinks=<',hrefsLinks,'>');
  }
  
  getTextAllChildren_(elem){
    if(elem.type === 'text') {
      //console.log('elem.data=<',elem.data,'>');
      this.plainText += elem.data;
    }
    if(elem.children && Array.isArray(elem.children)) {
      let self = this;
      //console.log('elem.children=<',elem.children,'>');
      elem.children.forEach( (valueA, indexA, arA) => {
        self.getTextAllChildren_(valueA);
      });
    }
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
    let counter = hrefsLinks.length;
    for(let i = 0;i < hrefsLinks.length ;i++) {
      let link = hrefsLinks[i];
      //console.log('link=<',link,'>');
      let hashLink = this.sha512_(link);
      //console.log('hashLink=<',hashLink,'>');
      let self = this;
      this.client.keys(redisKeyPrefixDone + '/' + hashLink, function (err, keys) {
        if (err) {
          console.log('err=<',err,'>');
          self.onApiError_();
          return;
        }
        //console.log('keys=<',keys,'>');
        if(keys.length === 0) {
          self.client.set(redisKeyPrefixTodo + '/' + hashLink, link);
        }
        counter -= 1;
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





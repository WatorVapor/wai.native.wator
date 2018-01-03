var request = require('request');
const cheerio = require('cheerio');
var redis = require('redis');
var crypto = require("crypto");
var ipfsAPI = require('ipfs-api');



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
    this.client = redis.createClient();
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
    this.client.keys(redisKeyPrefixTodo + '/*', function (err, keys) {
      if (err) {
        console.log('err=<',err,'>');
        return;
      }
      if(keys.length > 0) {
        console.log('keys[0]=<',keys[0],'>');
        self.client.get(keys[0], function (err, reply) {
        if (err) {
          console.log('err=<',err,'>');
          return;
        }
        console.log('reply=<',reply,'>');
          self.getOneTitle_(reply);
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
      if (error && response.statusCode !== 200) {
        console.log('error: error=<',error,'>');
        console.log('error: response=<',response,'>');
      } else {
        //console.log('body=<',body,'>');
        self.parseHTML_(body,url);
      }
    });
  }
  
  
  parseHTML_(data,url) {
    var plainText = '';
    var hrefsLinks = [];
    try {
      //console.log('data=<',data,'>');
      const $ = cheerio.load(data);
      $('p').each( (i, elem) => {
        //console.log('i=<',i,'>');
        //console.log('elem=<',elem,'>');
        elem.children.forEach( (value, index, ar) => {
          //console.log('value=<',value,'>');
          if(value.type === 'text') {
            //console.log('value.data=<',value.data,'>');
            plainText += value.data;
          }
          if(value.type === 'tag' && value.name === 'a') {
            //console.log('value=<',value,'>');
            value.children.forEach( (valueA, indexA, arA) => {
              if(valueA.type === 'text') {
                //console.log('valueA.data=<',valueA.data,'>');
                plainText += valueA.data;
              }
            });
            if(value.attribs.href.startsWith(this.prefix)) {
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
      });
    } catch(e) {
      console.log('e=<',e,'>');
    }
    this.saveDoneWiki_(url,plainText);
    this.saveLinkedWiki_(hrefsLinks);
    console.log('plainText=<',plainText,'>');
    //console.log('hrefsLinks=<',hrefsLinks,'>');
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
          return;
        }
        //console.log('keys=<',keys,'>');
        if(keys.length === 0) {
          self.client.set(redisKeyPrefixTodo + '/' + hashLink, link);
        }
        counter -= 1;
        console.log('counter=<',counter,'>');
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
  }
  
  sha512_(msg) {
    let sha512 = crypto.createHash('sha512');
    sha512.update(msg);
    let hash = sha512.digest('hex');
    return hash;
  }
}





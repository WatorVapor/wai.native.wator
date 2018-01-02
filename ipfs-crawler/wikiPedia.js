const https = require('https');
const cheerio = require('cheerio');
var redis = require('redis');
var crypto = require("crypto");


const redisKeyPrefix = '/wator/wai/crawler/wiki';
const redisKeyPrefixDone = '/wator/wai/crawler/wiki/done';
const redisKeyPrefixTodo = '/wator/wai/crawler/wiki/todo';

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
    this.client = redis.createClient();
    //console.log('WikiCrawler::constructor this=<',this,'>');
  }
  
  
  runOnce(){
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
    https.get(url, (resp) => {
      let data = '';
      resp.on('data', (chunk) => {
        data += chunk;
      });
      resp.on('end', () => {
        //console.log('end: data=<',data,'>');
        this.parseHTML_(data,url);
      });
    }).on('error', (err) => {
      console.log('Error: err=<',err,'>');
    });
  }
  
  parseHTML_(data,url) {
    var plainText = '';
    var hrefsLinks = [];
    try {
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
    let hashURL = this.sha512(url);
    console.log('url=<',url,'>');
    console.log('hashURL=<',hashURL,'>');
    
    //console.log('plainText=<',plainText,'>');
    //console.log('hrefsLinks=<',hrefsLinks,'>');
    for(let i = 0;i < hrefsLinks.length ;i++) {
      let link = hrefsLinks[0];
      console.log('link=<',link,'>');
      let hashLink = this.sha512(link);
      console.log('hashLink=<',hashLink,'>');
      this.client.set(redisKeyPrefixTodo + '/' + hashLink, link);
    }
  }
  
  sha512(msg) {
    let sha512 = crypto.createHash('sha512');
    sha512.update(msg);
    let hash = sha512.digest('hex');
    return hash;
  }
}





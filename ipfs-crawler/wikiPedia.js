const https = require('https');
const cheerio = require('cheerio');
var redis = require('redis');
var client = redis.createClient();


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
    console.log('WikiCrawler::constructor this=<',this,'>');
  }
  
  
  runOnce(){
    https.get(this.root + this.seed, (resp) => {
      let data = '';
      resp.on('data', (chunk) => {
        data += chunk;
      });
      resp.on('end', () => {
        //console.log('end: data=<',data,'>');
        this.parseHTML_(data);
      });
    }).on('error', (err) => {
      console.log('Error: err=<',err,'>');
    });
  }
  
  
  parseHTML_(data) {
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
    console.log('plainText=<',plainText,'>');
    console.log('hrefsLinks=<',hrefsLinks,'>');
  }    
}





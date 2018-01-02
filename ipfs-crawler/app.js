var ipfsAPI = require('ipfs-api');
var ipfs = ipfsAPI({host: 'localhost', port: '5001', protocol: 'http'});

var wiki = require('./wikiPedia.js');
var zhOption =  {
  prefix:'/wiki/',
  replace:'/zh-cn/',
  root:'https://zh.wikipedia.org',
  seed:'/zh-cn/%E5%8C%96%E5%AD%B8%E5%85%83%E7%B4%A0'
}
var zhWiki = new wiki(zhOption);
console.log('zhWiki=<',zhWiki,'>');


/*
ipfs.util.addFromFs('/tmp/test.txt',{ recursive: false},function(err, result) {
  if (err) {
    throw err;
  }
  console.log('result=<',result,'>');
});
*/

//console.log('client=<',client,'>');

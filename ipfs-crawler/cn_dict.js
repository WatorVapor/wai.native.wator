var wiki = require('./wikiDict.js');
var zhOption =  {
  root:'https://zh.wikipedia.org'
}
var zhWiki = new wiki(zhOption);
console.log('zhWiki=<',zhWiki,'>');

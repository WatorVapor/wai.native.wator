var wiki = require('./wikiDict.js');
var zhOption =  {
  root:'https://zh.wikipedia.org'
}
var zhWiki = new wiki(zhOption);
console.log('zhWiki=<',zhWiki,'>');

function onFinnish(){
  setTimeout(function(){
    zhWiki.runOnce(onFinnish);
  },1);
}
setTimeout(function(){
  zhWiki.runOnce(onFinnish);
},1000);

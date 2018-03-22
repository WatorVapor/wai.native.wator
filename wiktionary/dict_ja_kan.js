var wiki = require('./wikiDictJa.js');
var jaOption =  {
  root:'https://ja.wiktionary.org/wiki/'
}
var jaWiki = new wiki(jaOption);
console.log('jaWiki=<',jaWiki,'>');

function onFinnish(){
  setTimeout(function(){
    jaWiki.runOnce(onFinnish);
  },1);
}
setTimeout(function(){
  jaWiki.runOnce(onFinnish);
},1000);

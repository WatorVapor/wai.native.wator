const wiki = require('./parseWikiDumper.js');
let dumpPath = '/watorvapor/wai.storage/dumps.wikimedia.org/jawiktionary/jawiktionary-20180401-pages-articles.xml';
let wikiDumper = new wiki(dumpPath,onPage);

function onPage(title,text){
  console.log('onPage::title=<',title,'>');
  console.log('onPage::text=<',text,'>');
}

const wiki = require('./parseWikiDumper.js');
let dumpPath = '/watorvapor/wai.storage/dumps.wikimedia.org/zhwiktionary/zhwiktionary-20180401-pages-articles.xml';
let wikiDumper = new wiki(dumpPath,onPage);

function onPage(title,text){
  //console.log('onPage::title=<',title,'>');
  //console.log('onPage::text=<',text,'>');
  fetchByPronPinYin(title,text);
}
function fetchByPronPinYin(title,textPure) {
  console.log('fetchByPronPinYin::title=<',title,'>');
  let parma1 = textPure.split('{{漢語讀音');
  if(parma1.length > 1) {
    console.log('fetchByPronPinYin::parma1[1]=<',parma1[1],'>');
  }
}

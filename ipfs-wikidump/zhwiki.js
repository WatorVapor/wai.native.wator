const wiki = require('./parseWikiDumper.js');
let dumpPath = '/watorvapor/wai.storage/dumps.wikimedia.org/zhwiki/zhwiki-20180401-pages-articles.xml';
let wikiDumper = new wiki(dumpPath,onPage);

const level = require('level');
let dbPath = '/watorvapor/wai.storage/dumps.wikimedia.org/output_leveldb/zhwiki';
let db = level(dbPath);


function pushToDB(title,page) {
  console.log('pushToDB::title=<',title,'>');
  console.log('pushToDB::page=<',page,'>');
  db.put(title,page);
}

function onPage(title,text){
  let filters = [
    'Wikipedia:','Help:','Wikipedia:',
  ];
  /*
  if(filterTitle(filters,title)) {
    //console.log('onPage::filter out title=<',title,'>');
    return;
  }
  if(filterAscii(title)) {
    //console.log('onPage::filter out title=<',title,'>');
    return;
  }
  if(filterMaoHao(title)) {
    //console.log('onPage::filter out title=<',title,'>');
    return;
  }
  */
  console.log('onPage::title=<',title,'>');
  //console.log('onPage::text=<',text,'>');
}











function filterTitle(filters,title) {
  for(let i = 0;i < filters.length;i++) {
    let filter = filters[i];
    if(title.includes(filter)) {
      return true;
    }
  }
  return false;
}

function filterAscii(title) {
  for(let i = 0;i < title.length;i++) {
    let charCode = title.charAt(i);
    //console.log('filterAccii:charCode=<',charCode,'>');
    if(charCode >= '、') {
      return false;
    }
  }
  return true;
}
function filterMaoHao(title) {
  if(title.includes(':')) {
    return true;
  }
  if(title.includes('：')) {
    return true;
  }
  return false;
}


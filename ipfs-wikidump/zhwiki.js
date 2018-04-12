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
  console.log('onPage::title=<',title,'>');
  //console.log('onPage::text=<',text,'>');
}



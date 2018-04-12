const wiki = require('./parseWikiDumper.js');
let dumpPath = '/watorvapor/wai.storage/dumps.wikimedia.org/zhwiki/zhwiki-20180401-pages-articles.xml';
let wikiDumper = new wiki(dumpPath,onPage);

const level = require('level');
let dbPath = '/watorvapor/wai.storage/dumps.wikimedia.org/output_leveldb/zhwiki';
let db = level(dbPath);
const SHA3 = require('sha3');

const ArchiveRoot = '/watorvapor/wai.storage/dumps.wikimedia.org/output_hashindex/zhwiki';


function pushToDB(title,titleSha) {
  console.log('pushToDB::title=<',title,'>');
  console.log('pushToDB::titleSha=<',titleSha,'>');
  db.put(title,titleSha);
}

function onPage(title,text){
  let filters = [
    'Wikipedia:','Help:','Template:','Category:'
  ];
  if(filterTitle(filters,title)) {
    //console.log('onPage::filter out title=<',title,'>');
    return;
  }
  console.log('onPage::title=<',title,'>');
  let d = new SHA3.SHA3Hash(256);
  d.update(title);
  let titleSha = d.digest('hex');
  console.log('onPage::titleSha=<',titleSha,'>');
  let lvl1Path = titleSha.substr(0,3);
  let lvl2Path = titleSha.substr(3,3);
  let lvl3Path = titleSha.substr(6,3);
  let lvl4Path1 = titleSha.substr(9,3);
  let lvl5Path1 = titleSha.substr(12,3);
  let lvlAllPath = ArchiveRoot;
  lvlAllPath += '/' + lvl1Path;
  lvlAllPath += '/' + lvl2Path;
  lvlAllPath += '/' + lvl3Path;
  lvlAllPath += '/' + lvl4Path1;
  lvlAllPath += '/' + lvl5Path1;
  console.log('onPage::lvlAllPath=<',lvlAllPath,'>');
  let lvlFullPath = lvlAllPath + '/' + titleSha + '.txt';
  console.log('onPage::lvlFullPath=<',lvlFullPath,'>');
  
  //pushToDB(title,titleSha);
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



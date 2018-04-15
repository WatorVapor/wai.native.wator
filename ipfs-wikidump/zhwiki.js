const dumpPath = '/watorvapor/wai.storage/dumps.wikimedia.org/zhwiki/zhwiki-20180401-pages-articles.xml';
const dbPath = '/watorvapor/wai.storage/dumps.wikimedia.org/output_leveldb/zhwiki/title';
const ArchiveRoot = '/watorvapor/wai.storage/dumps.wikimedia.org/output_hashindex/zhwiki';



const wiki = require('./parseWikiDumper.js');
const level = require('level');
let db = level(dbPath);
const SHA3 = require('sha3');
const execSync = require('child_process').execSync;
const fs = require('fs');



function pushToDB(key,value) {
  //console.log('pushToDB::key=<',key,'>');
  //console.log('pushToDB::value=<',value,'>');
  db.put(key,value);
}

const ResumePosKey = 'wiki_dump_resume_pos';
let ResumePos = 0;
db.get(ResumePosKey, function (err, value) {
  if (!err) {
    console.log('db.get::value=<',value,'>');
    ResumePos = parseInt(value);
    if(ResumePos > 1024) {
      ResumePos -= 1024;
    }
  }
  setTimeout(function(){
    let wikiDumper = new wiki(dumpPath,ResumePos,onPage);
  },1);
})


function onPage(title,pos,text){
  if(!title) {
    return;
  }
  let filters = [
    'Wikipedia:','Help:','Template:','Category:','MediaWiki:','Hex',
    'File:','Portal:'
  ];
  if(filterTitle(filters,title)) {
    //console.log('onPage::filter out title=<',title,'>');
    return;
  }
  console.log('onPage::title=<',title,'>');
  let d = new SHA3.SHA3Hash(256);
  d.update(title);
  let titleSha = d.digest('hex');
  //console.log('onPage::titleSha=<',titleSha,'>');
  pushToDB(title,titleSha);
  let lvl1Path = titleSha.substr(0,4);
  let lvl2Path = titleSha.substr(4,4);
  let lvl3Path = titleSha.substr(8,4);
  let lvl4Path1 = titleSha.substr(12,4);
  let lvl5Path1 = titleSha.substr(16,4);
  let lvlAllPath = ArchiveRoot;
  lvlAllPath += '/' + lvl1Path;
  lvlAllPath += '/' + lvl2Path;
  lvlAllPath += '/' + lvl3Path;
  lvlAllPath += '/' + lvl4Path1;
  lvlAllPath += '/' + lvl5Path1;
  //console.log('onPage::lvlAllPath=<',lvlAllPath,'>');
  execSync('mkdir -p ' + lvlAllPath);
  let lvlFullPath = lvlAllPath + '/' + titleSha + '.txt';
  //console.log('onPage::lvlFullPath=<',lvlFullPath,'>');
  fs.writeFileSync(lvlFullPath,text);
  pushToDB(ResumePosKey,pos);
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



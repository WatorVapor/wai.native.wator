const dumpPath = '/watorvapor/wai.storage/dumps.wikimedia.org/jawiki/jawiki-20180420-pages-articles.xml';
const dbTitlePath = '/watorvapor/wai.storage/dumps.wikimedia.org/output_leveldb/jawiki/title';
const dbPagePath = '/watorvapor/wai.storage/dumps.wikimedia.org/output_leveldb/jawiki/page';

const skipTitles = [
    'Wikipedia:','Help:','Template:','Category:','MediaWiki:','Hex',
    'HEX','File:','Portal:','ファイル:'
];


const wiki = require('./parseWikiDumper.js');
const level = require('level');
let dbTittle = level(dbTitlePath);
let dbPage = level(dbPagePath);
const SHA3 = require('sha3');
const execSync = require('child_process').execSync;
const fs = require('fs');



function pushTitle2DB(key,value) {
  //console.log('pushToDB::key=<',key,'>');
  //console.log('pushToDB::value=<',value,'>');
  dbTittle.put(key,value);
}

function pushPage2DB(key,value) {
  //console.log('pushToDB::key=<',key,'>');
  //console.log('pushToDB::value=<',value,'>');
  dbPage.put(key,value);
}


const ResumePosKey = 'wiki_dump_resume_pos';
let ResumePos = 0;
dbTittle.get(ResumePosKey, function (err, value) {
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
  if(filterTitle(skipTitles,title)) {
    //console.log('onPage::filter out title=<',title,'>');
    return;
  }
  console.log('onPage::title=<',title,'>');
  let d = new SHA3.SHA3Hash(256);
  d.update(title);
  let titleSha = d.digest('hex');
  //console.log('onPage::titleSha=<',titleSha,'>');
  pushPage2DB(titleSha,text);
  pushTitle2DB(title,titleSha);
  pushTitle2DB(ResumePosKey,pos);
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


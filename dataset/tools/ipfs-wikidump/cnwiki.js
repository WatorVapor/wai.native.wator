const dumpPath = '/watorvapor/wai.storage/dumps.wikimedia.org/zhwiki/zhwiki-20180420-pages-articles.xml';
const dbTitlePath = '/watorvapor/wai.storage/dumps.wikimedia.org/output_leveldb/cnwiki/title';
const dbPagePath = '/watorvapor/wai.storage/dumps.wikimedia.org/output_leveldb/cnwiki/page';

let skipTitles = [
  'Wikipedia:','Help:','Template:','Category:','MediaWiki:','Hex',
  'File:','Portal:','模块:',
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

function pushPage2DB(key,value,cnTitle,zhTitle) {
  //console.log('pushPage2DB::key=<',key,'>');
  //console.log('pushPage2DB::value=<',value,'>');
  dbPage.get(key,function (err, valueOld) {
    if (err) {
      if (err.notFound) {
        //console.log('pushPage2DB:: new cnTitle=<',cnTitle,'>');
        dbPage.put(key,value);
      }
    } else {
      //console.log('pushPage2DB::cnTitle=<',cnTitle,'>');
      //console.log('pushPage2DB::zhTitle=<',zhTitle,'>');
      //console.log('pushPage2DB::valueOld=<',valueOld.length,'>');
      //console.log('pushPage2DB::value=<',value.length,'>');
      if(value.length > valueOld.length) {
        dbPage.put(key,value);
      }
    }
  });
}

const dbTitblePagePath = '/watorvapor/wai.storage/dumps.wikimedia.org/output_leveldb/cnwiki/titlePage';
let dbTitlePage = level(dbTitblePagePath);

async function pushTitlePage2DB(key,value,cnTitle,zhTitle) {
  //console.log('pushPage2DB::key=<',key,'>');
  //console.log('pushPage2DB::value=<',value,'>');
  await dbTitlePage.get(key,function (err, valueOld) {
    if (err) {
      if (err.notFound) {
        //console.log('pushPage2DB:: new cnTitle=<',cnTitle,'>');
        dbTitlePage.put(key,value);
      }
    } else {
      //console.log('pushPage2DB::cnTitle=<',cnTitle,'>');
      //console.log('pushPage2DB::zhTitle=<',zhTitle,'>');
      //console.log('pushPage2DB::valueOld=<',valueOld.length,'>');
      //console.log('pushPage2DB::value=<',value.length,'>');
      if(value.length > valueOld.length) {
        dbTitlePage.put(key,value);
      }
    }
  });
}

function pushPosTitlePage2DB(key,value) {
  //console.log('pushToDB::key=<',key,'>');
  //console.log('pushToDB::value=<',value,'>');
  dbTitlePage.put(key,value);
}



const ResumePosKey = 'wiki_dump_resume_pos';
let ResumePos = 0;
dbTitlePage.get(ResumePosKey, function (err, value) {
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


const opencc = require('node-opencc');

function onPage(zhTitle,pos,zhText){
  if(!zhTitle) {
    return;
  }
  if(filterTitle(skipTitles,zhTitle)) {
    //console.log('onPage::filter out zhTitle=<',zhTitle,'>');
    return;
  }
  //console.log('onPage::zhTitle=<',zhTitle,'>');
  let cnTitle = opencc.traditionalToSimplified(zhTitle);
  let cnText = opencc.traditionalToSimplified(zhText);
  //console.log('onPage::cnText=<',cnText,'>');
  pushTitlePage2DB(cnTitle,cnText,cnTitle,zhTitle);
  pushPosTitlePage2DB(ResumePosKey,pos);
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



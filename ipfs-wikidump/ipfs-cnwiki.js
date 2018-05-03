const dumpPath = '/watorvapor/wai.storage/dumps.wikimedia.org/zhwiki/zhwiki-20180420-pages-articles.xml';

let skipTitles = [
  'Wikipedia:','Help:','Template:','Category:','MediaWiki:','Hex',
  'File:','Portal:','模块:',
];

const wiki = require('./parseWikiDumper.js');
const level = require('level');




const dbPath = '/watorvapor/wai.storage/dumps.wikimedia.org/output_leveldb/cnwiki/ipfs';
let db = level(dbPath);

async function pushIpfs2DB(key,value,cnTitle,zhTitle) {
  //console.log('pushPage2DB::key=<',key,'>');
  //console.log('pushPage2DB::value=<',value,'>');
  await db.get(key,function (err, valueOld) {
    if (err) {
      if (err.notFound) {
        //console.log('pushPage2DB:: new cnTitle=<',cnTitle,'>');
        db.put(key,value);
      }
    } else {
      //console.log('pushPage2DB::cnTitle=<',cnTitle,'>');
      //console.log('pushPage2DB::zhTitle=<',zhTitle,'>');
      //console.log('pushPage2DB::valueOld=<',valueOld.length,'>');
      //console.log('pushPage2DB::value=<',value.length,'>');
      if(value.length > valueOld.length) {
        db.put(key,value);
      }
    }
  });
}

function pushPosIpfs2DB(key,value) {
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
  console.log('onPage::cnText=<',cnText,'>');
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



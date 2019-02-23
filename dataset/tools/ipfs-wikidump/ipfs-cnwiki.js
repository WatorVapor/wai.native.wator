const dumpPath = '/watorvapor/wai.storage/dumps.wikimedia.org/zhwiki/zhwiki-20190201-pages-articles.xml';
const dbPath = '/ceph/storage3/wai.storage/dumps.wikimedia.org/output_leveldb/cnwiki/ipfs';

let skipTitles = [
  'Wikipedia:','Help:','Template:','Category:','MediaWiki:','Hex',
  'File:','Portal:','模块:',
];

const wiki = require('./parseWikiDumper.js');
const leveldown = require('leveldown');
const IpfsSave = require('./ipfs-save.js');
const LevelSave = require('./ipfs-level.js');

let wikiDumper = false;
let iSave = new IpfsSave();
let lSave = new LevelSave(dbPath,(ResumePos)=> {
  console.log('::ResumePos=<',ResumePos,'>');
  wikiDumper = new wiki(dumpPath,ResumePos,onPage);
});

iSave.onError = (err) => {
  lSave.close();
}

const opencc = require('node-opencc');
let iSavePageCounter = 0;

function onPage(zhTitle,pos,zhText){
  //console.log('onPage::zhTitle=<',zhTitle,'>');
  if(!zhTitle) {
    if(wikiDumper) {
      wikiDumper.resume();
    }
    return;
  }
  if(filterTitle(skipTitles,zhTitle)) {
    //console.log('onPage::filter out zhTitle=<',zhTitle,'>');
    if(wikiDumper) {
      wikiDumper.resume();
    }
    return;
  }
  //console.log('onPage::zhTitle=<',zhTitle,'>');
  let cnTitle = opencc.traditionalToSimplified(zhTitle);
  let cnText = opencc.traditionalToSimplified(zhText);
  //console.log('onPage::cnText=<',cnText,'>');
  iSave.save(cnTitle,cnText,pos,(hash) => {
    lSave.save(hash,cnText,pos,()=> {
      wikiDumper.resume();
    });
  });
}




process.on('exit', (code) => {
  console.log('exit ::code=<',code,'>');
  lSave.close();
  console.log('exit ::code=<',code,'>');
});

/*
process.on('SIGINT', (code) => {
  console.log('SIGINT ::code=<',code,'>');
  lSave.close();
});
process.on('SIGTERM', (code) => {
  console.log('SIGTERM ::code=<',code,'>');
  lSave.close();
});
*/

/*
process.on('SIGKILL', (code) => {
  console.log('SIGKILL ::code=<',code,'>');
  lSave.close();
});
*/



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



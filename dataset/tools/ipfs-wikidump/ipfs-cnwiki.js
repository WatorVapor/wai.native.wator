const dumpPath = '/watorvapor/wai.storage/dumps.wikimedia.org/zhwiki/zhwiki-20190201-pages-articles.xml';
const dbPath = '/watorvapor/wai.storage/dumps.wikimedia.org/output_leveldb/cnwiki/ipfs';
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
let lSave = new LevelSave(()=> {
  wikiDumper = new wiki(dumpPath,ResumePos,onPage);
});

const opencc = require('node-opencc');
let iSavePageCounter = 0;
const iConstSavePageOneTime = 2000;

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
    lSave.save(cnTitle,cnText,pos,()=> {
      wikiDumper.resume();
    });
  });
}




process.on('exit', (code) => {
  console.log('exit ::code=<',code,'>');
  runCloseDB();
  console.log('exit ::code=<',code,'>');
});


process.on('SIGINT', (code) => {
  console.log('SIGINT ::code=<',code,'>');
  runCloseDB();
});
process.on('SIGTERM', (code) => {
  console.log('SIGTERM ::code=<',code,'>');
  runCloseDB();
});
/*
process.on('SIGKILL', (code) => {
  console.log('SIGKILL ::code=<',code,'>');
  runCloseDB();
});
*/


runCloseDB = () => {
  db.close( (evt)=> {
    console.log('db.close save2Ipfs::evt=<',evt,'>');
    process.exit(0);
  });
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



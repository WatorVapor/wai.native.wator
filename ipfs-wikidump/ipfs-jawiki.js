const dumpPath = '/watorvapor/wai.storage/dumps.wikimedia.org/jawiki/jawiki-20180420-pages-articles.xml';
const dbPath = '/watorvapor/wai.storage/dumps.wikimedia.org/output_leveldb/jawiki/ipfs';
const skipTitles = [
    'Wikipedia:','Help:','Template:','Category:','MediaWiki:','Hex',
    'HEX','File:','Portal:','ファイル:'
];


const wiki = require('./parseWikiDumper.js');
const level = require('level');
const ipfsAPI = require('ipfs-api');
const ipfs = ipfsAPI({host: 'localhost', port: '5002', protocol: 'http'});



ipfs.id(function (err, identity) {
  if (err) {
    throw err;
  }
  console.log('ipfs.id identity=<',identity,'>');
});





let db = level(dbPath);
let wikiDumper = false;


async function pushIpfs2DB(key,value) {
  //console.log('pushPage2DB::key=<',key,'>');
  //console.log('pushPage2DB::value=<',value,'>');
  db.put(key,value);
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
    wikiDumper = new wiki(dumpPath,ResumePos,onPage);
  },1);
})

function onPage(title,pos,text){
  if(!title) {
    if(wikiDumper) {
      wikiDumper.resume();
    }
    return;
  }
  if(filterTitle(skipTitles,title)) {
    //console.log('onPage::filter out title=<',title,'>');
    if(wikiDumper) {
      wikiDumper.resume();
    }
    return;
  }
  console.log('onPage::title=<',title,'>');
  //console.log('onPage::titleSha=<',titleSha,'>');
  save2Ipfs(title,text,pos);
}



function save2Ipfs(cnTitle,cnText,pos) {
  let bufText = Buffer.from(cnText, 'utf8');
  ipfs.files.add(bufText,function(err, result) {
    if (err) {
      console.log('save2Ipfs::err=<',err,'>');
      process.exit(0);
      return;
    }
    //console.log('save2Ipfs::result=<',result,'>');
    let hash = result[0].hash;
    //console.log('save2Ipfs::hash=<',hash,'>');
    //console.log('save2Ipfs::cnTitle=<',cnTitle,'>');
    //console.log('save2Ipfs::pos=<',pos,'>');
    if(hash) {
      pushIpfs2DB(hash,cnTitle);
      pushIpfs2DB(ResumePosKey,pos);
      if(wikiDumper) {
        wikiDumper.resume();
      }
    }
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


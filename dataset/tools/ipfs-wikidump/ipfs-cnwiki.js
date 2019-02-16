const dumpPath = '/watorvapor/wai.storage/dumps.wikimedia.org/zhwiki/zhwiki-20180501-pages-articles.xml';
const dbPath = '/watorvapor/wai.storage/dumps.wikimedia.org/output_leveldb/cnwiki/ipfs';
let skipTitles = [
  'Wikipedia:','Help:','Template:','Category:','MediaWiki:','Hex',
  'File:','Portal:','模块:',
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


const opencc = require('node-opencc');

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
  save2Ipfs(cnTitle,cnText,pos);
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



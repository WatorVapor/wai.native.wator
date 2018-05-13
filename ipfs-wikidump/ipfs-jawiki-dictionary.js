const dbPath = '/watorvapor/wai.storage/dumps.wikimedia.org/output_leveldb/jawiki/ipfs';
const dbDictPath = '/watorvapor/wai.storage/dumps.wikimedia.org/output_leveldb/jawiki/dict';

const level = require('level');
let dbDict = level(dbDictPath);
const lastWikiPosition = 'last-wiki-position';
let lastWiki = false;
dbDict.get(lastWikiPosition, function (err, value) {
  if (err && err.notFound) {
    setTimeout(function(){
      startReadWiki();
    },1);
  } else {
    console.log('lastWikiPosition value=<',value,'>');
    lastWiki = value;
    setTimeout(function(){
      startReadWiki();
    },1);
  }
});

let db = level(dbPath);
let stream = false;
function startReadWiki() {
  console.log('startReadWiki lastWiki=<',lastWiki,'>');
  if(lastWiki) {
    stream = db.createReadStream({gt:lastWiki});
  } else {
    stream = db.createReadStream();
  }
  stream.on('data', function (data) {
    //console.log(data.key.toString('utf-8'), '=', data.value.toString('utf-8'));
    //console.log('data.key=<',data.key.toString('utf-8'),'>');
    //console.log('data.value=<',data.value.toString('utf-8'),'>');
    if(data.key.toString('utf-8') !== lastWikiPosition) {
      stream.pause();
      readIpfsInfo(data.key.toString('utf-8'));
    }
  });
  stream.on('error', function (err) {
    console.log('Oh my!', err);
  });
  stream.on('close', function () {
    console.log('Stream closed');
  });
  stream.on('end', function () {
    console.log('Stream ended');
    writeBlock();
  });
}


const ipfsAPI = require('ipfs-api');
const ipfs = ipfsAPI({host: 'localhost', port: '5002', protocol: 'http'});

ipfs.id(function (err, identity) {
  if (err) {
    throw err;
  }
  console.log('ipfs.id identity=<',identity,'>');
});

function readIpfsInfo(path) {
  //console.log('readIpfsInfo::path=<',path,'>');
  ipfs.files.get(path, function (err, files) {
    //console.log('readIpfsInfo::files=<',files,'>');
    files.forEach((file) => {
      //console.log('readIpfsInfo::file=<',file,'>');
      //console.log('readIpfsInfo::file.content.length=<',file.content.length,'>');
      //console.log('readIpfsInfo::file.content=<',file.content.toString('utf8'),'>');
      onIpfsWikiText(file.content.toString('utf8'));
    });
  });  
}

function onIpfsWikiText(text) {
  console.log('onIpfsWikiText::text=<',text,'>');
  let param1 = text.split("'''（");
  console.log('onIpfsWikiText::param1.length=<',param1.length,'>');
}




async function pushDict2DB(key,value) {
  //console.log('pushDict2DB::key=<',key,'>');
  //console.log('pushDict2DB::value=<',value,'>');
  dbDict.put(key,value);
}



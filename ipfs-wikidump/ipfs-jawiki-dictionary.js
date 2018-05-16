const dbPath = '/watorvapor/wai.storage/dumps.wikimedia.org/output_leveldb/jawiki/ipfs';
const dbDictPath = '/watorvapor/wai.storage/dumps.wikimedia.org/output_leveldb/jawiki/dict';

const level = require('level');
let dbDict = level(dbDictPath);
const lastWikiPosition = 'last-wiki-position';
let lastWiki = false;
dbDict.get(lastWikiPosition, function (err, value) {
  if (err && err.notFound) {
    console.log('lastWikiPosition err=<',err,'>');
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
    if (err) {
      throw err;
    }
    //console.log('readIpfsInfo::files=<',files,'>');
    files.forEach((file) => {
      //console.log('readIpfsInfo::file=<',file,'>');
      //console.log('readIpfsInfo::file.content.length=<',file.content.length,'>');
      //console.log('readIpfsInfo::file.content=<',file.content.toString('utf8'),'>');
      onIpfsWikiText(file.content.toString('utf8'),path);
    });
    stream.resume();
  });
}


function onIpfsWikiText(text,path) {
  //console.log('onIpfsWikiText::text=<',text,'>');
  
  
  let results = text.match(/'''(.*)'''\((.*)\)/g);
  //console.log('onIpfsWikiText::results=<',results,'>');
  if(results && results.length > 0) {
    for(let i = 0;i < results.length;i++) {
      let param1 = results[i];
      //console.log('onIpfsWikiText::param1=<',param1,'>');
      let param2 = param1.split("'''(");
      if(param2.length > 1) {
        let title = param2[0].replace("'''",'');
        let param3 = param2[1].split(")");
        if(param3.length > 1) {
          let hiraCandidate = param3[0];
          //console.log('onIpfsWikiText::title=<',title,'>');
          //console.log('onIpfsWikiText::hiraCandidate=<',hiraCandidate,'>');
          pushDict2DB(title,hiraCandidate,path);
          pushIndex2DB(lastWikiPosition,path);
        }
      }
    }
    return;
  }
  
  results = text.match(/'''(.*)'''（(.*)）/g);
  //console.log('onIpfsWikiText::results=<',results,'>');
  if(results && results.length > 0) {
    for(let i = 0;i < results.length;i++) {
      let param1 = results[i];
      //console.log('onIpfsWikiText::param1=<',param1,'>');
      let param2 = param1.split("'''（");
      if(param2.length > 1) {
        let title = param2[0].replace("'''",'');
        let param3 = param2[1].split("）");
        if(param3.length > 1) {
          let hiraCandidate = param3[0];
          //console.log('onIpfsWikiText::title=<',title,'>');
          //console.log('onIpfsWikiText::hiraCandidate=<',hiraCandidate,'>');
          pushDict2DB(title,hiraCandidate,path);
          pushIndex2DB(lastWikiPosition,path);
        }
      }
    }
    return;
  }  

  results = text.match(/'''(.*)'''」（(.*)）/g);
  //console.log('onIpfsWikiText::results=<',results,'>');
  if(results && results.length > 0) {
    for(let i = 0;i < results.length;i++) {
      let param1 = results[i];
      //console.log('onIpfsWikiText::param1=<',param1,'>');
      let param2 = param1.split("'''」（");
      if(param2.length > 1) {
        let title = param2[0].replace("'''",'');;
        let param3 = param2[1].split("）");
        if(param3.length > 1) {
          let hiraCandidate = param3[0];
          //console.log('onIpfsWikiText::title=<',title,'>');
          //console.log('onIpfsWikiText::hiraCandidate=<',hiraCandidate,'>');
          pushDict2DB(title,hiraCandidate,path);
          pushIndex2DB(lastWikiPosition,path);
        }
      }
    }
    return;
  }  

  results = text.match(/'''(.*)'''』（(.*)）/g);
  //console.log('onIpfsWikiText::results=<',results,'>');
  if(results && results.length > 0) {
    for(let i = 0;i < results.length;i++) {
      let param1 = results[i];
      //console.log('onIpfsWikiText::param1=<',param1,'>');
      let param2 = param1.split("'''』（");
      if(param2.length > 1) {
        let title = param2[0].replace("'''",'');;
        let param3 = param2[1].split("）");
        if(param3.length > 1) {
          let hiraCandidate = param3[0];
          //console.log('onIpfsWikiText::title=<',title,'>');
          //console.log('onIpfsWikiText::hiraCandidate=<',hiraCandidate,'>');
          pushDict2DB(title,hiraCandidate,path);
          pushIndex2DB(lastWikiPosition,path);
        }
      }
    }
    return;
  }  
  
  
  results = text.match(/'''(.*)'''　（(.*)）/g);
  //console.log('onIpfsWikiText::results=<',results,'>');
  if(results && results.length > 0) {
    for(let i = 0;i < results.length;i++) {
      let param1 = results[i];
      //console.log('onIpfsWikiText::param1=<',param1,'>');
      let param2 = param1.split("'''　（");
      if(param2.length > 1) {
        let title = param2[0].replace("'''",'');;
        let param3 = param2[1].split("）");
        if(param3.length > 1) {
          let hiraCandidate = param3[0];
          //console.log('onIpfsWikiText::title=<',title,'>');
          //console.log('onIpfsWikiText::hiraCandidate=<',hiraCandidate,'>');
          pushDict2DB(title,hiraCandidate,path);
          pushIndex2DB(lastWikiPosition,path);
        }
      }
    }
    return;
  }  

  results = text.match(/'''(.*)'''}}』（(.*)）/g);
  //console.log('onIpfsWikiText::results=<',results,'>');
  if(results && results.length > 0) {
    for(let i = 0;i < results.length;i++) {
      let param1 = results[i];
      //console.log('onIpfsWikiText::param1=<',param1,'>');
      let param2 = param1.split("'''}}』（");
      if(param2.length > 1) {
        let title = param2[0].replace("'''",'');;
        let param3 = param2[1].split("）");
        if(param3.length > 1) {
          let hiraCandidate = param3[0];
          //console.log('onIpfsWikiText::title=<',title,'>');
          //console.log('onIpfsWikiText::hiraCandidate=<',hiraCandidate,'>');
          pushDict2DB(title,hiraCandidate,path);
          pushIndex2DB(lastWikiPosition,path);
        }
      }
    }
    return;
  }
  //console.log('onIpfsWikiText::text=<',text,'>');
  if(filterText(text)) {
    pushDict2DB(lastWikiPosition,path);
  } else {
    //pushIndex2DB(lastWikiPosition,path);
  }
}

function filterText(text) {
  if(text.startsWith('#転送[[')) {
    return true;
  }
  if(text.startsWith('#転送 [[')) {
    return true;
  }
  if(text.startsWith('#REDIRECT [[')) {
    return true;
  }
  if(text.startsWith('#REDIRECT[[')) {
    return true;
  }
  if(text.startsWith('return {')) {
    return true;
  }
  return false;
}



async function pushDict2DB(key,value,path) {
  //console.log('pushDict2DB::key=<',key,'>');
  //console.log('pushDict2DB::value=<',value,'>');
  let newVal = refineValue(key,value);
  if(newVal) {
    console.log('pushDict2DB::key=<',key,'>');
    console.log('pushDict2DB::newVal=<',newVal,'>');
    if(path) {
      console.log('pushDict2DB::path=<',path,'>');
    }
    dbDict.put(key,newVal);
  }
}

async function pushIndex2DB(key,value,path) {
  //console.log('pushIndex2DB::key=<',key,'>');
  //console.log('pushIndex2DB::value=<',value,'>');
  dbDict.put(key,value);
}


const furikana = require('./furikan.json');
function refineValue(key,value) {
  if(isAllFurikana(key)) {
    return null;
  }
  let param1 = value.split('、');
  let pureHira = param1[0].replace(/'''/g,'');
  if(isAllFurikana(pureHira)) {
    return pureHira;
  }
  console.error('refineValue::pureHira=<',pureHira,'>');
  console.error('refineValue::key=<',key,'>');
}
function isAllFurikana(key) {
  //console.log('isAllFurikana::key=<',key,'>');
  for(let i = 0;i < key.length ;i++) {
    let code = key[i];
    if(!furikana[code]) {
      return false;
    }
  }
  return true;
}


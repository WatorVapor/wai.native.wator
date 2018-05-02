const fs = require('fs')
const cheerio = require('cheerio');
const readline = require('readline');

const Iconv  = require('iconv').Iconv;
let iconv = new Iconv('UTF-32', 'UTF-8');
console.log('iconv=<',iconv,'>');



const unihanPath = '/watorvapor/wai.storage/www.unicode.org/ucd.unihan.flat.xml';
let stream = fs.createReadStream(unihanPath, "utf8");
let rl = readline.createInterface({'input': stream, 'output': {}});


let codeStart = false;
let codeXML = '';
rl.on('line', (line) => {
  if(line.includes('<char')) {
    codeStart = true; 
  }
  if(codeStart) {
    codeXML += line;
    codeXML += '\n';
  }
  if(line.includes('/>')) {
    codeStart = false;
    parseCode_(codeXML);
    codeXML = '';
  }
});



function parseCode_(xml_data) {
  //console.log('parseCode_:xml_data=<',xml_data,'>');
  const $ = cheerio.load(xml_data);
  $('char').each( (i, elem) => {
    //console.log('parseCode_:elem=<',elem,'>');
    //console.log('parseCode_:elem.attribs=<',elem.attribs,'>');
    if(elem.attribs) {
      let khanyupinyin = elem.attribs.khanyupinyin;
      let cp = getUtf8(elem.attribs.cp);
      if( cp && khanyupinyin ) {
        saveHanYuPinYin(khanyupinyin,cp);
      }
      let on = elem.attribs.kjapaneseon;
      if(cp && on) {
        saveJapaneseOn(on,cp);
      }
      let kun = elem.attribs.kjapanesekun;
      if(cp && kun) {
        saveJapaneseKun(kun,cp);
      }
    }
  });
}

function getUtf8(unicode) {
  //console.log('getUtf8:unicode=<',unicode,'>');
  let utf32 = parseInt(unicode,16);
  //console.log('getUtf8:utf32=<',utf32,'>');
  let buf = new Buffer(4);
  buf.writeUInt32BE(utf32, 0);
  //console.log('buf=<',buf,'>');
  let utf8 = iconv.convert(buf);
  //console.log('utf8=<',utf8,'>');  
  //console.log('utf8=<',utf8.toString(),'>');
  return utf8.toString();
}

const level = require('level');
let dbPathPinyin = '/watorvapor/wai.storage/www.unicode.org/output_leveldb/pinyin';
let dbPinyin = level(dbPathPinyin);


function saveHanYuPinYin(pinYin,utf8) {
  //console.log('saveHanYuPinYin:utf8=<',utf8,'>');
  //console.log('saveHanYuPinYin:pinYin=<',pinYin,'>');
  let pinYins = pinYin.split(':');
  if(pinYins.length > 1) {
    let purePY = pinYins[1];
    //console.log('saveHanYuPinYin:purePY=<',purePY,'>');
    dbPinyin.put(utf8,purePY);
  }
}


let dbPathJaOn = '/watorvapor/wai.storage/www.unicode.org/output_leveldb/jaOn';
let dbJaOn = level(dbPathJaOn);


function saveJapaneseOn(on,utf8) {
  //console.log('saveJapaneseOn:utf8=<',utf8,'>');
  //console.log('saveJapaneseOn:on=<',on,'>');
  let kanas = getKanas(on,utf8);
  //console.log('saveJapaneseOn:kanas=<',kanas,'>');
  dbJaOn.put(utf8,kanas);
}

let dbPathJaKun = '/watorvapor/wai.storage/www.unicode.org/output_leveldb/jaKun';
let dbJaKun = level(dbPathJaKun);

function saveJapaneseKun(kun,utf8) {
  //console.log('saveJapaneseKun:utf8=<',utf8,'>');
  //console.log('saveJapaneseKun:kun=<',kun,'>');
  let kanas = getKanas(kun,utf8);
  //console.log('saveJapaneseKun:kanas=<',kanas,'>');
  dbJaKun.put(utf8,kanas);
}

let romaDB = require('./unihan_ja_roma.js')
//console.log('romaDB=<',romaDB,'>');


function getKanas(romas,utf8) {
  let lowerOn = romas.toLowerCase();
  //console.log('saveJapanese:lowerOn=<',lowerOn,'>');
  let lowerOns = lowerOn.split(' ');
  let kanas = []
  for(let i = 0;i < lowerOns.length;i++) {
    let onJa = lowerOns[i];
    let kana = roma2kana(onJa,utf8);
    kanas.push(kana);
  }
  return kanas;
}


function roma2kana(romaStr,utf8) {
  let temp='';
  let kanas='';
  for(let i = 0;i < romaStr.length;i++) {
    temp += romaStr[i];
    //console.log('roma2kana:temp=<',temp,'>');
    let kana = romaDB.roma[temp];
    //console.log('roma2kana:kana=<',kana,'>');
    if(romaStr[i] === 'n') {
      if(i+1 < romaStr.length) {
        let next = romaStr[i+1];
        if(next === 'y' 
           || next === 'a' 
           || next === 'i' 
           || next === 'u' 
           || next === 'e' 
           || next === 'o') {
           continue;
        }
      }
    }
    if(kana) {
      kanas += kana;
      temp = '';
    }
  }
  if(!kanas) {
    console.log('roma2kana:utf8=<',utf8,'>');
    console.log('roma2kana:romaStr=<',romaStr,'>');
    console.log('roma2kana:kanas=<',kanas,'>');
  }
  //console.log('roma2kana:romaStr=<',romaStr,'>');
  //console.log('roma2kana:kanas=<',kanas,'>');
  return kanas;
}

getKanas('HAN BAN');
getKanas('OOKII HAJIME OOINI');
getKanas('KAWARU KAERU');
getKanas('JU NYU');
getKanas('HANA');


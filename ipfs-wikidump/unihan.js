const fs = require('fs')
const cheerio = require('cheerio');
const readline = require('readline');

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
      let cp = elem.attribs.cp;
      if(khanyupinyin && cp) {
        saveHanYuPinYin(khanyupinyin,cp);
      }
      let kJapaneseOn = elem.attribs.kjapaneseon;
      let kJapaneseKun = elem.attribs.kjapanesekun;
      if(kJapaneseOn && kJapaneseKun && cp) {
        saveJapanese(kun,on,cp);
      }
    }
  });
}

function saveHanYuPinYin(pinYin,unicode) {
  //console.log('saveHanYuPinYin:pinYin=<',pinYin,'>');
  //console.log('saveHanYuPinYin:unicode=<',unicode,'>');
}

function saveJapanese(kun,on,unicode) {
  console.log('saveJapanese:kun=<',kun,'>');
  console.log('saveJapanese:on=<',on,'>');
  console.log('saveJapanese:unicode=<',unicode,'>');
}


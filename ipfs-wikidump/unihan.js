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
    console.log('parseCode_:elem.attribs=<',elem.attribs,'>');
  });
}


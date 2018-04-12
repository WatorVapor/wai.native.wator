const fs = require('fs')
const cheerio = require('cheerio');

const unihanPath = '/watorvapor/wai.storage/www.unicode.org/ucd.unihan.flat.xml';
let xml_data = fs.readFileSync(unihanPath, "utf-8");
const $ = cheerio.load(xml_data);

$('description').each( (i, elem) => {
  let textElem = elem.children[0];
  if(textElem.type === 'text') {
    let length = textElem.data.length;
    let description = textElem.data.substr(0,length);
    console.log('description=<',description,'>');
  }
});


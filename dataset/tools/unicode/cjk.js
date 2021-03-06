const UNICODE = require('./unicode.js');
const Iconv  = require('iconv').Iconv;
let iconv = new Iconv('UTF-32', 'UTF-8');
console.log('iconv=<',iconv,'>');

//console.log('UNICODE=<',UNICODE,'>');

let cjkRange = [
  'cjkRadicals','cjkSymbolsPunctuation','cjkStrokes','enclosedCJKLettersMonths','cjkCompatibility',
  'cjk','cjkA','cjkA2','cjkB','cjkC','cjkD','cjkE','cjkF',
  'cjkCompatibilityIdeographs'
]
//console.log('cjkRange=<',cjkRange,'>');
let cjkJson = {};

for(let index in cjkRange.reverse()) {
  let key = cjkRange[index];
  //console.log('key=<',key,'>');
  let range = UNICODE[key];
  //console.log('range=<',range,'>');
  for(let utf32 = range[1];utf32 >= range[0] ;utf32--) {
    utf32to8(utf32)
  }
}


function utf32to8(utf32) {
  //console.log('utf32=<',utf32,'>');
  let buf = Buffer.alloc(4);
  buf.writeUInt32BE(utf32, 0);
  //console.log('buf=<',buf,'>');
  let utf8 = iconv.convert(buf);
  //console.log('utf8=<',utf8,'>');  
  //console.log('utf8=<',utf8.toString(),'>');
  let key = utf8.toString();
  cjkJson[key] = true;
}
//console.log('cjkJson=<',JSON.stringify(cjkJson,2,' '),'>');

const fs=require("fs");
fs.writeFileSync('cjk.json',JSON.stringify(cjkJson,2,' '))

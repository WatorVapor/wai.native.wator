const UNICODE = require('./orig.unicode.js');
//console.log('UNICODE=<',UNICODE,'>');
let keys = Object.keys(UNICODE);
//console.log('keys=<',keys,'>');

let genUnicode = 'const UNICODE = { \n';
for(let i = 1;i < keys.length;i++) {
  let key = keys[i-1];
  //console.log('key=<',key,'>');
  let keyNext = keys[i];
  let start = UNICODE[key];
  let end = UNICODE[keyNext] -1;
  //console.log('start=<',start,'>');
  //console.log('end=<',end,'>');
  //console.log('  ',key,':',start,end);
  genUnicode += '  ' + key + ':  [0x' + start.toString(16) + ',  0x' + end.toString(16) + ']';
  genUnicode += ',\n';
}
genUnicode += '}\n';

console.log('genUnicode=<',genUnicode,'>');


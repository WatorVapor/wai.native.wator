const UNICODE = require('./unicode.js');
const Iconv  = require('iconv').Iconv;
let iconv = new Iconv('UTF-32', 'UTF-8');
console.log('iconv=<',iconv,'>');

//console.log('UNICODE=<',UNICODE,'>');

let cjkRange = [
  'cjk','cjkA','cjkB','cjkC','cjkD','cjkE','cjkF'
]
//console.log('cjkRange=<',cjkRange,'>');

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
  let buf = new Buffer(4);
  buf.writeUInt32BE(utf32, 0);
  //console.log('buf=<',buf,'>');
  let utf8 = iconv.convert(buf);
  //console.log('utf8=<',utf8,'>');  
  console.log('utf8=<',utf8.toString(),'>');
}


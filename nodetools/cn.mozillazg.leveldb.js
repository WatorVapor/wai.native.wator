var levelup = require('levelup');

var fs = require('fs');
var dataPinYin = fs.readFileSync('pinyin.txt', {
  encoding: 'utf-8'
});
//console.log('dataPinYin =<',dataPinYin,'>');
var pinYinLine = dataPinYin.split('\n');
//console.log('pinYinLine =<',pinYinLine,'>');
pinYinLine.forEach(function(line) {
  if(!line.startsWith('#')) {
    //console.log('line =<',line,'>');
    let param1 = line.split(':');
    console.log('param1[1] =<',param1[1],'>');
    let param2 = param1[1].split('#');
    console.log('param2[0] =<',param2[0],'>');
    console.log('param2[1] =<',param2[1],'>');
  }
});

var phonemeDB = {};
console.log('phonemeDB =<',phonemeDB,'>');

var distDB = '../db/tts/cn/hanzi'
//var dist = levelup(distDB);

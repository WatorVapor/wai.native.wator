var levelup = require('levelup');

var fs = require('fs');
var dataPinYin = fs.readFileSync('pinyin.txt', {
  encoding: 'utf-8'
});
//console.log('dataPinYin =<',dataPinYin,'>');
var pinYinLine = dataPinYin.split('\n');
//console.log('pinYinLine =<',pinYinLine,'>');
pinYinLine.forEach(function(line) {
  if(line.startsWith('#')) {
    console.log('line =<',line,'>');
  }
});

var phonemeDB = {};
console.log('phonemeDB =<',phonemeDB,'>');

var distDB = '../db/tts/cn/hanzi'
//var dist = levelup(distDB);

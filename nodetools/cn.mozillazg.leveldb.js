var levelup = require('levelup');

var fs = require('fs');
var dataPinYin = fs.readFileSync('pinyin.txt', {
    encoding: 'utf-8'
});
console.log('data =<',dataPinYin,'>');

var phonemeDB = {};
console.log('phonemeDB =<',phonemeDB,'>');

var distDB = '../db/tts/cn/hanzi'
//var dist = levelup(distDB);

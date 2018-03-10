var levelup = require('levelup');

var fs = require('fs');
var data = fs.readFileSync('pinyin.txt', {
    encoding: 'utf-8'
});
console.log('data =<',data,'>');

var phonemeDB = {};
console.log('phonemeDB =<',phonemeDB,'>');

var distDB = '../db/tts/cn/hanzi'
//var dist = levelup(distDB);

var levelup = require('levelup');
var phonemePath = '/watorvapor/rsaauth/wator/pubKeys/b3f3e38d032b4b956d3c4591f40aaff21fb337ec1340626eaf3b35e84e23ced763c18502925f0a16d5b4a10284b83d5e15cbc1cbcec593e095c0776d287e8f36/wai/phoneme_cn.json';
var phonemeDB = require(phonemePath);
console.log('phonemeDB =<',phonemeDB,'>');

//var argv = require('argv');
//var args = argv.run();
//var srcJson = args.targets[0];
//console.log('srcJson =<',srcJson,'>');

//var distDB = args.targets[1];
//console.log('distDB =<',distDB,'>');
var distDB = '../db/tts/cn/hanzi'
var dist = levelup(distDB);
var hanziDB = {};
if(phonemeDB.cn) {
  phonemeDB.cn.forEach(function(phoneme){
    //console.log('phoneme =<',phoneme,'>');
    let hanzis = phoneme.cn_help.split(' ');
    //console.log('hanzis =<',hanzis,'>');
    hanzis.forEach(function(hanzi){
      //console.log('hanzi =<',hanzi,'>');
      tryAddHanzi(hanzi,phoneme.phoneme);
    });
    //dist.put(phoneme.phoneme,phoneme.ipfs);
  });
}

//console.log('hanziDB =<',hanziDB,'>');

let keysHanzi = Object.keys(hanziDB);
keysHanzi.forEach(function(hanzi){
  //console.log('hanzi =<',hanzi,'>');
  let phoneme = hanziDB[hanzi];
  //console.log('phoneme =<',phoneme,'>');
  dist.put(hanzi,phoneme);
});


function tryAddHanzi(hanzi,phoneme) {
  if(! hanziDB[hanzi]) {
    hanziDB[hanzi] = phoneme;
  } else {
    hanziDB[hanzi] += ',' + phoneme;
  }
}

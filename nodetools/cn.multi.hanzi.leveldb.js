var levelup = require('levelup');
var phonemePath = '/watorvapor/rsaauth/wator/pubKeys/b3f3e38d032b4b956d3c4591f40aaff21fb337ec1340626eaf3b35e84e23ced763c18502925f0a16d5b4a10284b83d5e15cbc1cbcec593e095c0776d287e8f36/wai/phoneme_cn.json';
var phonemeDB = require(phonemePath);

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
  });
}

var multiHanziDB = {};

function tryAddHanzi(hanzi,phoneme) {
  if(! hanziDB[hanzi]) {
    hanziDB[hanzi] = phoneme;
  } else {
    hanziDB[hanzi] += ',' + phoneme;
  }
  
  if(! multiHanziDB[hanzi]) {
    multiHanziDB[hanzi] = [phoneme];
  } else {
    multiHanziDB[hanzi].push(phoneme);
  }
}
console.log('multiHanziDB =<',multiHanziDB,'>');


var levelup = require('levelup');

var distDB = '../db/tts/ja/hanzi'
var dist = levelup(distDB);

var phonemePath = '/watorvapor/rsaauth/wator/pubKeys/b3f3e38d032b4b956d3c4591f40aaff21fb337ec1340626eaf3b35e84e23ced763c18502925f0a16d5b4a10284b83d5e15cbc1cbcec593e095c0776d287e8f36/wai/phoneme_ja.json';
var phonemeDB = require(phonemePath);
console.log('phonemeDB =<',phonemeDB,'>');


if(phonemeDB.ja) {
  phonemeDB.ja.forEach(function(phoneme){
    console.log('phoneme =<',phoneme,'>');
    dist.put(phoneme.phoneme,phoneme.phoneme);
  });
}

var levelup = require('levelup');
var phonemePath = '/watorvapor/rsaauth/wator/pubKeys/';
phonemePath += 'b3f3e38d032b4b956d3c4591f40aaff21fb337ec1340626eaf3b35e84e23ced763c18502925f0a16d5b4a10284b83d5e15cbc1cbcec593e095c0776d287e8f36';
phonemePath += '/wai/phoneme_ctrl.json';
var phonemeDB = require(phonemePath);
console.log('phonemeDB =<',phonemeDB,'>');

var distDB = '../db/tts/ja/phoneme'
var dist = levelup(distDB);

var distDB2 = '../db/tts/cn/phoneme'
var dist2 = levelup(distDB2);

if(phonemeDB.ctrl) {
  phonemeDB.ctrl.forEach(function(phoneme){
    console.log('phoneme =<',phoneme,'>');
    if(phoneme.phoneme) {
      dist.put(phoneme.phoneme,phoneme.ipfs);
      dist2.put(phoneme.phoneme,phoneme.ipfs);
    }
  });
}

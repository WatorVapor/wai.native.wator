var levelup = require('levelup');
var phonemePath = '/watorvapor/rsaauth/wator/pubKeys/';
phonemePath += 'b3f3e38d032b4b956d3c4591f40aaff21fb337ec1340626eaf3b35e84e23ced763c18502925f0a16d5b4a10284b83d5e15cbc1cbcec593e095c0776d287e8f36';
phonemePath += '/wai/phoneme_ext_ja.json';
var phonemeDB = require(phonemePath);
console.log('phonemeDB =<',phonemeDB,'>');

//var argv = require('argv');
//var args = argv.run();
//var srcJson = args.targets[0];
//console.log('srcJson =<',srcJson,'>');

//var distDB = args.targets[1];
//console.log('distDB =<',distDB,'>');
var distDB = '../db/tts/ja/phoneme'
var dist = levelup(distDB);
if(phonemeDB.ext_ja) {
  phonemeDB.ext_ja.forEach(function(phoneme){
    console.log('phoneme =<',phoneme,'>');
    dist.put(phoneme.phoneme,phoneme.ipfs);
  });
}

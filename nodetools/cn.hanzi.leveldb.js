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

function tryAddHanzi(hanzi,phoneme) {
  //console.log('hanzi =<',hanzi,'>');
  //console.log('phoneme =<',phoneme,'>');
  dist.get(hanzi, function (err, value) {
    if (err) {
      if (err.notFound) {
        dist.set(hanzi,value + ',' + phoneme);
      } else {
        console.log('tryAddHanzi:err =<',err,'>');
      }
    } else {
      console.log('tryAddHanzi:value =<',value,'>');
      dist.set(hanzi,value + ',' + phoneme);
    }
  });
}



var levelup = require('levelup');
var phonemePath = '/watorvapor/rsaauth/wator/pubKeys/b3f3e38d032b4b956d3c4591f40aaff21fb337ec1340626eaf3b35e84e23ced763c18502925f0a16d5b4a10284b83d5e15cbc1cbcec593e095c0776d287e8f36/wai/phoneme_ja.json';
var phoneme = require(phonemePath);
console.log('phoneme =<',phoneme,'>');

//var argv = require('argv');
//var args = argv.run();
//var srcJson = args.targets[0];
//console.log('srcJson =<',srcJson,'>');

//var distDB = args.targets[1];
//console.log('distDB =<',distDB,'>');
var dist = levelup(distDB);



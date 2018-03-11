var levelup = require('levelup');

var distDB = '../db/tts/ja/hanzi'
var dist = levelup(distDB);

var Kana = [
  {あ:'あ'}
];
console.log('Kana =<',Kana,'>');

Kana.forEach(function(jakan,phoneme){
  console.log('jakan =<',jakan,'>');
  console.log('phoneme =<',phoneme,'>');
  dist.put(jakan,phoneme);
});



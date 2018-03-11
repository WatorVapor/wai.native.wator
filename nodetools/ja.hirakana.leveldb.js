var levelup = require('levelup');

var distDB = '../db/tts/ja/hanzi'
var dist = levelup(distDB);

var Kana = {
  あ:'あ',
};


Kana.forEach(function(Kana,phoneme){
  console.log('Kana =<',Kana,'>');
  console.log('phoneme =<',phoneme,'>');
  dist.put(Kana,phoneme);
});



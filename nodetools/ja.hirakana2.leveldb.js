var levelup = require('levelup');

var distDB = '../db/tts/ja/hanzi'
var dist = levelup(distDB);


dist.put('ぁ','ぁ');
dist.put('ぃ','ぃ');
dist.put('ぅ','ぅ');
dist.put('ぇ','ぇ');
dist.put('ぉ','ぉ');

dist.put('っ','っ');

dist.put('ゃ','ゃ');
dist.put('ゅ','ゅ');
dist.put('ょ','ょ');

dist.put('ゎ','ゎ');

dist.put('ヵ','ヵ');
dist.put('ヶ','ヶ');

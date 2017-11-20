var levelup = require('levelup');
var argv = require('argv');
var args = argv.run();
var srcDB = args.targets[0];
console.log('srcDB =<',srcDB,'>');
var distDB = args.targets[1];
console.log('distDB =<',distDB,'>');


var dist = levelup(distDB);

var src = levelup(srcDB);
src.createReadStream()
  .on('data', function (data) {
    var value = parseInt(data.value);
    if(value > 2) {
    //if(true) {
      console.log(data.key, '=', data.value)
    }
  })
  .on('error', function (err) {
    console.log('Oh my!', err)
  })
  .on('close', function () {
    console.log('Stream closed')
  })
  .on('end', function () {
    console.log('Stream ended')
  })

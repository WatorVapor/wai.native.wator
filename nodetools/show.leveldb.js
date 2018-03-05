var levelup = require('levelup');
var argv = require('argv');
var args = argv.run();
var pathDB = args.targets[0];
console.log('pathDB =<',pathDB,'>');


var db = levelup(pathDB);
db.createReadStream()
  .on('data', function (data) {
    var value = parseInt(data.value);
    console.log(data.key, '=', data.value)
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

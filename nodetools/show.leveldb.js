var levelup = require('levelup');
var argv = require('argv');
var args = argv.run();
var pathDB = args.targets[0];
console.log('pathDB =<',pathDB,'>');


var db = levelup(leveldown(pathDB));
var stream = db.createReadStream();

var counter = 0;
stream.on('data', function (data) {
  console.log(data.key, '=', data.value);
  console.log('data.key=<',data.key,'>')
  console.log('data.value=<',data.value,'>')
  counter++
});
stream.on('error', function (err) {
  console.log('Oh my!', err)
});
stream.on('close', function () {
  console.log('Stream closed')
});
stream.on('end', function () {
  console.log('Stream ended')
  console.log('counter=<',counter,'>')
});

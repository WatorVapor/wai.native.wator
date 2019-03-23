const argv = require('argv');
const args = argv.run();
const pathDB = args.targets[0];
console.log('pathDB =<',pathDB,'>');

const level = require('level');

const db = level(pathDB);
const stream = db.createReadStream();

let counter = 0;
stream.on('data', function (data) {
  //console.log(data.key.toString('utf-8'), '=', data.value.toString('utf-8'));
  console.log('data.key=<',data.key.toString('utf-8'),'>')
  console.log('data.value=<',data.value.toString('utf-8'),'>')
  counter++
  stream.pause();
});
stream.on('error', function (err) {
  console.log('error err=<',err,'>')
});
stream.on('close', function () {
  console.log('Stream closed')
});
stream.on('end', function () {
  console.log('Stream ended')
  console.log('counter=<',counter,'>')
});


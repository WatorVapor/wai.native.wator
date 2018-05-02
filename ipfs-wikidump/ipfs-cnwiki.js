const level = require('level');
let dbPathTitle = '/watorvapor/wai.storage/dumps.wikimedia.org/output_leveldb/cnwiki';
var dbTitle = level(dbPathTitle);
var stream = dbTitle.createReadStream();


var counter = 0;
stream.on('data', function (data) {
  //console.log(data.key.toString('utf-8'), '=', data.value.toString('utf-8'));
  //console.log('data.key=<',data.key.toString('utf-8'),'>')
  //console.log('data.value=<',data.value.toString('utf-8'),'>')
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

const level = require('level');
let dbPathTitle = '/watorvapor/wai.storage/dumps.wikimedia.org/output_leveldb/cnwiki/title';
var dbTitle = level(dbPathTitle);
var stream = dbTitle.createReadStream();



var counter = 0;
stream.on('data', function (data) {
  //console.log(data.key.toString('utf-8'), '=', data.value.toString('utf-8'));
  let title = data.key.toString('utf-8');
  let keyPage = data.value.toString('utf-8');
  GetPage(title,keyPage);
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

let dbPathPage = '/watorvapor/wai.storage/dumps.wikimedia.org/output_leveldb/cnwiki/page';
var dbPage = level(dbPathPage);

function GetPage(title,key) {
  console.log('GetPage::title=<',title,'>');
  console.log('GetPage::key=<',key,'>');
  dbPage.get(key, function (err, value) {
    if (err) {
      console.log('GetPage::err=<',err,'>')
      return;
    }
    console.log('GetPage::value=<',value,'>');
  });
}

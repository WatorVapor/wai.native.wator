const dbZHTitlePath = '/watorvapor/wai.storage/dumps.wikimedia.org/output_leveldb/zhwiki/title';
const dbZHPagePath = '/watorvapor/wai.storage/dumps.wikimedia.org/output_leveldb/zhwiki/page';
const dbCNTitlePath = '/watorvapor/wai.storage/dumps.wikimedia.org/output_leveldb/cnwiki/title';
const dbCNPagePath = '/watorvapor/wai.storage/dumps.wikimedia.org/output_leveldb/cnwiki/page';


const level = require('level');
let dbZHTittle = level(dbZHTitlePath);
let dbZHPage = level(dbZHPagePath);
const SHA3 = require('sha3');
const opencc = require('node-opencc');


let stream = dbZHTittle.createReadStream();
let counter = 0;
stream.on('data', async function (data) {
  let title = data.key.toString('utf-8');
  let pageIndex = data.value.toString('utf-8');
  //console.log('pageIndex=<',pageIndex,'>');
  await dbZHPage.get(pageIndex, function (err, value) {
    if (err) {
      console.log('err=<',err,'>');
    } else {
      saveZh2CN(title,value);
    }
  });
  counter++;
});
stream.on('error', function (err) {
  console.log('Oh my!', err);
});
stream.on('close', function () {
  console.log('Stream closed');
});
stream.on('end', function () {
  console.log('Stream ended');
  console.log('counter=<',counter,'>');
});

function saveZh2CN(zhTitle,zhPage) {
  let cnTitle = opencc.traditionalToSimplified(zhTitle);
  console.log('zhTitle=<',zhTitle,'>');
  console.log('cnTitle=<',cnTitle,'>');
  let cnPage = opencc.traditionalToSimplified(zhPage);
  console.log('zhPage=<',zhPage,'>');
  console.log('cnPage=<',cnPage,'>');
}


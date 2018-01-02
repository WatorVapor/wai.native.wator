const cheerio = require('cheerio');
const https = require('https');

https.get('https://zh.wikipedia.org/zh-cn/%E5%8E%9F%E5%AD%90', (resp) => {
  let data = '';
  resp.on('data', (chunk) => {
    data += chunk;
  });
  resp.on('end', () => {
    //console.log('end: data=<',data,'>');
    const $ = cheerio.load(data);
    let pAll = $('p');
    console.log('pAll=<',pAll,'>');
  });
}).on('error', (err) => {
  console.log('Error: err=<',err,'>');
});

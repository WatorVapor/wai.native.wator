const cheerio = require('cheerio');
const https = require('https');

https.get('', (resp) => {
  let data = '';
  resp.on('data', (chunk) => {
    data += chunk;
  });
  resp.on('end', () => {
    console.log('end: data=<',data,'>');
  });
}).on('error', (err) => {
  console.log('Error: err=<',err,'>');
});

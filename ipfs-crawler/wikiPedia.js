const https = require('https');

https.get('https://zh.wikipedia.org/zh-cn/%E5%8E%9F%E5%AD%90', (resp) => {
  let data = '';
  resp.on('data', (chunk) => {
    data += chunk;
  });
  resp.on('end', () => {
    //console.log('end: data=<',data,'>');
    parseHTML(data);
  });
}).on('error', (err) => {
  console.log('Error: err=<',err,'>');
});

const cheerio = require('cheerio');
function parseHTML($data) {
  try {
    const $ = cheerio.load(data);
    $('p').each( (i, elem)=> {
      console.log('i=<',i,'>');
      console.log('elem=<',elem,'>');
      elem.children.forEach(function(value, index, ar){
        console.log('value=<',value,'>');
        console.log('index=<',index,'>');
        console.log('ar=<',ar,'>');
      });
    });
  } catch(e) {
    console.log('e=<',e,'>');
  }
}

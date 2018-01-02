const https = require('https');
const hrefPrefix = '/wiki/';
const hrefReplace = '/zh-cn/';


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

function parseHTML(data) {
  var plainText = '';
  var hrefsLinks = [];
  try {
    const $ = cheerio.load(data);
    $('p').each( (i, elem) => {
      //console.log('i=<',i,'>');
      //console.log('elem=<',elem,'>');
      elem.children.forEach( (value, index, ar) => {
        //console.log('value=<',value,'>');
        if(value.type === 'text') {
          //console.log('value.data=<',value.data,'>');
          plainText += value.data;
        }
        if(value.type === 'tag' && value.name === 'a') {
          //console.log('value=<',value,'>');
          value.children.forEach( (valueA, indexA, arA) => {
            if(valueA.type === 'text') {
              //console.log('valueA.data=<',valueA.data,'>');
              plainText += valueA.data;
            }
          });
          if(value.attribs.href.startsWith(hrefPrefix)) {
            if(hrefReplace) {
              hrefsLinks.push(value.attribs.href);
            } else {
              let newHref = value.attribs.href.replace(hrefPrefix,hrefReplace);
              console.log('newHref=<',newHref,'>');
              hrefsLinks.push(newHref);
            }
          } else {
            console.log('value.attribs.href=<',value.attribs.href,'>');
          }
        }
        //console.log('index=<',index,'>');
      });
    });
  } catch(e) {
    console.log('e=<',e,'>');
  }
  console.log('plainText=<',plainText,'>');
  console.log('hrefsLinks=<',hrefsLinks,'>');
}

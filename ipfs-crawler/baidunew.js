const request = require('request');
const cheerio = require('cheerio');

const requestParam = {
  url: "http://news.baidu.com"
};

onHttpRequest = (error, response, body) => {
  if(error) {
    throw error;
  }
  if(response.statusCode !== 200) {
    throw response.statusCode;
  }
  //console.log('onHttpRequest::body=<',body,'>');
  onHttpBody(body);
}
request.get(requestParam,onHttpRequest);


onHttpBody= (body) => {
  const $ = cheerio.load(body);
  let link = $('a');
  //console.log('onHttpBody::link=<',link,'>');
  let linkKey = Object.keys(link);
  console.log('onHttpBody::linkKey=<',linkKey,'>');
  for(let i = 0;i < linkKey.length;i++) {
    let key = linkKey[i];
    let linkOne = link[key];
    //console.log('onHttpBody::linkOne=<',linkOne,'>');
    if(linkOne.attribs && linkOne.attribs.href) {
      let href = linkOne.attribs.href;
      if(href.startsWith('http://') || href.startsWith('https://')) {
        console.log('onHttpBody::href=<',href,'>');
      }
    }
  }
}

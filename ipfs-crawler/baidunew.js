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
  console.log('onHttpRequest::body=<',body,'>');
}

request.get(requestParam,onHttpRequest);

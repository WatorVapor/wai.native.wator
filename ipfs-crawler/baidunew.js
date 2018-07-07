const request = require('request');
const cheerio = require('cheerio');

const requestParam = {
  url: "http://news.baidu.com"
};
request.get(requestParam,onHttpRequest);

onHttpRequest(error, response, body) {
  console.log('onHttpRequest::error=<',error,'>');
  console.log('onHttpRequest::response=<',response,'>');
  console.log('onHttpRequest::body=<',body,'>');
}

var bot = require('nodemw');
// pass configuration object
var clientJa = new bot({
  protocol: 'https',           // Wikipedia now enforces HTTPS
  server: 'ja.wikipedia.org',  // host name of MediaWiki-powered site
  path: '/w',                  // path to api.php script
  debug: false                 // is more verbose when set to true
});

/*
clientJa.getBacklinks('日本', function(err, data) {
  // error handling
  if (err) {
    console.error('err=<',err,'>');
    return;
  }
  //console.log('data=<',data,'>');
  for(var i = 0;i < data.length;i++) {
    var page = data[i];
    console.log('page=<',page,'>');
  }
});
*/

var clientZh = new bot({
  protocol: 'https',           // Wikipedia now enforces HTTPS
  server: 'zh.wikipedia.org',  // host name of MediaWiki-powered site
  path: '/w',                  // path to api.php script
  debug: false                 // is more verbose when set to true
});

/*
clientZh.getArticle('中国', function(err, data) {
  // error handling
  if (err) {
    console.error('err=<',err,'>');
    return;
  }
  console.log('data=<',data,'>');
});
*/

clientZh.fetchUrl('https://zh.wikipedia.org/zh-cn/%E6%AC%A1%E5%8E%9F%E5%AD%90%E7%B2%92%E5%AD%90', function(err, data) {
  // error handling
  if (err) {
    console.error('err=<',err,'>');
    return;
  }
  console.log('data=<',data,'>');
});


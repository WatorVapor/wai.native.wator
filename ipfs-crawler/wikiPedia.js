var bot = require('nodemw');
// pass configuration object
var client = new bot({
  protocol: 'https',           // Wikipedia now enforces HTTPS
  server: 'en.wikipedia.org',  // host name of MediaWiki-powered site
  path: '/w',                  // path to api.php script
  debug: false                 // is more verbose when set to true
});

client.getArticle('foo', function(err, data) {
  // error handling
  if (err) {
    console.error('err=<',err,'>');
    return;
  }
  console.log('data=<',data,'>')
});

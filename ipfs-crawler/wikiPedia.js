var bot = require('nodemw');
// pass configuration object
var client = new bot({
  protocol: 'https',           // Wikipedia now enforces HTTPS
  server: 'ja.wikipedia.org',  // host name of MediaWiki-powered site
  path: '/w',                  // path to api.php script
  debug: false                 // is more verbose when set to true
});

client.getBacklinks('日本', function(err, data) {
  // error handling
  if (err) {
    console.error('err=<',err,'>');
    return;
  }
  console.log('data=<',data,'>');
  for(var i = 0;i < data.length;i++) {
    var page = data[i];
    console.log('page=<',page,'>');
  }
});

var levelup = require('levelup')
var db = levelup('/watorvapor/wai.storage/train/ostrich/cp.dict/cn')
db.createReadStream()
  .on('data', function (data) {
    var value = parseInt(data.value);
    if(value > 2) {
      console.log(data.key, '=', data.value)
    }
  })
  .on('error', function (err) {
    console.log('Oh my!', err)
  })
  .on('close', function () {
    console.log('Stream closed')
  })
  .on('end', function () {
    console.log('Stream ended')
  })


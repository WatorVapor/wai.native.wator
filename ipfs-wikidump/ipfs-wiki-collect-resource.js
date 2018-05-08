const dbPath = '/watorvapor/wai.storage/dumps.wikimedia.org/output_leveldb/cnwiki/ipfs';

const level = require('level');
const ipfsAPI = require('ipfs-api');
const ipfs = ipfsAPI({host: 'localhost', port: '5002', protocol: 'http'});

ipfs.id(function (err, identity) {
  if (err) {
    throw err;
  }
  console.log('ipfs.id identity=<',identity,'>');
});



let db = level(dbPath);
let stream = db.createReadStream();
var counter = 0;
stream.on('data', function (data) {
  //console.log(data.key.toString('utf-8'), '=', data.value.toString('utf-8'));
  console.log('data.key=<',data.key.toString('utf-8'),'>');
  console.log('data.value=<',data.value.toString('utf-8'),'>');
  stream.pause();
  readIpfsInfo(data.key.toString('utf-8'));
  counter++
});
stream.on('error', function (err) {
  console.log('Oh my!', err)
});
stream.on('close', function () {
  console.log('Stream closed')
});
stream.on('end', function () {
  console.log('Stream ended')
  console.log('counter=<',counter,'>')
});

function readIpfsInfo(path) {
  console.log('readIpfsInfo::path=<',path,'>');
  
  ipfs.ls(path, function (err, files) {
    console.log('readIpfsInfo::err=<',err,'>');
    console.log('readIpfsInfo::files=<',files,'>');
  });
  
/*
  ipfs.files.get(path, function (err, files) {
    console.log('readIpfsInfo::files=<',files,'>');
  });

  ipfs.files.stat('/ipfs/' + path, (err, stats) => {
    console.log('readIpfsInfo::err=<',err,'>');
    console.log('readIpfsInfo::stats=<',stats,'>');
  });
  
  ipfs.files.ls('/ipfs/' + path, function (err, files) {
    console.log('readIpfsInfo::ls err=<',err,'>');
    console.log('readIpfsInfo::ls files=<',files,'>');
  });
*/  

  const stream = ipfs.files.getReadableStream(path);
  console.log('readIpfsInfo::stream=<',stream,'>');
  let size = stream.readableLength();
  console.log('readIpfsInfo::size=<',size,'>');
  /*
  stream.on('data', (file) => {
    console.log('readIpfsInfo::file=<',file,'>');
    console.log('readIpfsInfo::file.content=<',file.content,'>');
  });
  */
}

const ipfsAPI = require('ipfs-http-client');
const fs=require("fs");
const Level = require('level');

const cids = require('./cnwikidump.refs.local.json');
const dbPath = './cnwiki-cid';
db = Level(dbPath);
for(let i = 0;i < cids.length;i++) {
  let cidObj = cids[i];
  //console.log('cidObj=<',cidObj,'>');
  let cid = cidObj.Ref;
  console.log('cid=<',cid,'>');
  db.batch()
    .put(hash, title)
    .put(ResumePosKey, pos)
    .write( (err)=>{
      if(err) {
        throw err;
      }
    });
}

const dbPath = '/watorvapor/wai.storage/dumps.wikimedia.org/output_leveldb/cnwiki/ipfs';
const leveldown = require('leveldown');
const ResumePosKey = 'wiki_dump_resume_pos';

module.exports = class LevelSave {
  constructor(cb) {
    this.db = leveldown(dbPath);
    this.cb = cb;
    let self = this;
    this.db.open( (evt) => {
      console.log('db.open::evt=<',evt,'>');
    });
  }
  save(hash,title,pos,cb) {
    let self = this;
    this.levelSaveCounter = 2;
    this.db.put(hash,title,{ sync: true },()=>{ 
      self._onLevelPut(cb);
    });
    this.db.put(ResumePosKey,pos,{ sync: true },()=>{ 
      self._onLevelPut(cb);
    );
  }
  _onLevelPut = (cb) =>{
    //console.log('_onLevelPut::cb=<',cb,'>');
    this.levelSaveCounter--;
    if(this.levelSaveCounter <= 0) {
      if(typeof cb === 'function') {
        cb();
      }
    }
  }  
  
  _onDBOpen() {
    db.get(ResumePosKey, function (err, value) {
      if (err) {
        throw err;
      }
      console.log('db.get::value=<',value,'>');
      let ResumePos = parseInt(value);
      if(ResumePos > 1024) {
        ResumePos -= 1024;
      }
      setTimeout(function(){
        if(typeof this.cb === 'function') {
          this.cb(ResumePos);
        }
      },1);
    })    
  }
}




const Level = require('level');
const ResumePosKey = 'wiki_dump_resume_pos';

module.exports = class LevelSave {
  constructor(dbPath,cb) {
    console.log('LevelSave::dbPath=<',dbPath,'>');
    this.db = Level(dbPath);
    this.cb = cb;
    let self = this;
    this.db.open( (evt) => {
      //console.log('db.open::evt=<',evt,'>');
      self._onDBOpen();
    });
  }
  save(hash,title,pos,cb) {
    let self = this;
    this.levelSaveCounter = 2;
    /*
    this.db.put(hash,title,{ sync: true },()=>{ 
      console.log('LevelSave save hash=<',hash,'>');
      self._onLevelPut(cb);
    });
    this.db.put(ResumePosKey,pos,{ sync: true },()=>{ 
      console.log('LevelSave save pos=<',pos,'>');
      self._onLevelPut(cb);
    });
    */
    this.db.batch()
    .put(hash, title)
    .put(ResumePosKey, pos)
    .write( ()=>{
      if(typeof cb === 'function') {
        cb();
      }
    });
  }
  close() {
    this.db.close( (evt)=> {
      console.log('close evt=<',evt,'>');
      process.exit(0);
    });
  }
  
  
  _onLevelPut(cb){
    //console.log('_onLevelPut::cb=<',cb,'>');
    this.levelSaveCounter--;
    if(this.levelSaveCounter <= 0) {
      if(typeof cb === 'function') {
        console.log('LevelSave _onLevelPut this.levelSaveCounter=<',this.levelSaveCounter,'>');
        cb();
      }
    }
  }  
  
  _onDBOpen() {
    let self = this;
    this.db.get(ResumePosKey, function (err, value) {
      let ResumePos = 0;
      if (!err) {
        console.log('_onDBOpen db.get::value=<',value.toString(),'>');
        ResumePos = parseInt(value.toString());
      }
      if(ResumePos > 1024) {
        ResumePos -= 1024;
      }
      console.log('_onDBOpen db.get::ResumePos=<',ResumePos,'>');
      setTimeout(function(){
        if(typeof self.cb === 'function') {
          self.cb(ResumePos);
        }
      },1);
    })
  }
}




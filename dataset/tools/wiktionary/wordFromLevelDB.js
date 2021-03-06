var LevelDown = require('leveldown');
var levelUp = require('levelup');
var level = require('level');

module.exports = class WordDBUtility {
  constructor(option) {
    if(option.input) {
      //console.log('constructor::option.input=<',option.input,'>');
      this.input = LevelDown(option.input);
      console.log('constructor::this.input=<',this.input,'>');
      let self = this;
      this.input.open(function(error){
        if(error) {
          console.log('constructor::error=<',error,'>');
        } else {
          self.inputIt = self.input.iterator();
        }
      });
      //this.inputIt = this.input.iterator();
      //console.log('constructor::this.inputIt=<',this.inputIt,'>');
    }
    if(option.output) {
      try {
        //console.log('constructor::option.output=<',option.output,'>');
        this.output = level(option.output);
        //console.log('constructor::this.output=<',this.output,'>');
      } catch(e) {
        console.log('constructor::e=<',e,'>');
      }
    }
     if(option.done) {
      try {
        //console.log('constructor::option.done=<',option.done,'>');
        this.done = level(option.done);
        //console.log('constructor::this.done=<',this.done,'>');
        let self= this;
        this.done.get('lastDone', function (err, value) {
          if(err) {
          } else {
            self.lastDone = value.toString('utf8');
          }
        });
      } catch(e) {
        console.log('constructor::e=<',e,'>');
      }
    }
   this.dry = false;
    if(option.dry) {
      this.dry = option.dry;
    }
    //console.log('constructor::this.input=<',this.input,'>');
    //console.log('constructor::this.output=<',this.output,'>');
    //console.log('constructor::this.done=<',this.done,'>');
    let timer = setInterval(function(){
      if(this.inputIt && this.lastDone) {
        this.inputIt.seek(this.lastDone);
        clearInterval(timer);
      }
    }.bind(this),1000)
  }
    
  getWord(cb){
    this.cb = cb;
    //console.log('getWord::this.inputIt=<',this.inputIt,'>');
    if(!this.inputIt) {
      return 'prepare';
    }
    let self = this;
    this.inputIt.next(function(error,key,value){
      if(error) {
        console.log('getWord::error=<',error,'>');
        cb('error');
      } else {
        //console.log('getWord::key=<',key,'>');
        //console.log('getWord::key.toString=<',key.toString('utf8'),'>');
        //console.log('getWord::value=<',value,'>');
        cb(key.toString('utf8'));
        self.prev = key.toString('utf8');
      }
    });
    if(this.prev) {
      this.done.put(this.prev,'true');
      this.done.put('lastDone',this.prev);
    }
    return 'ready';
  }
  
  setWordKana(word,kana) {
    this.output.put(word,kana);
  }
  setWordPinYin(word,pinYin) {
    this.output.put(word,pinYin);
  }
}

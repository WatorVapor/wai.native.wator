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
    this.dry = false;
    if(option.dry) {
      this.dry = option.dry;
    }
    //console.log('constructor::this.input=<',this.input,'>');
    //console.log('constructor::this.output=<',this.output,'>');
  }
    
  getWord(cb){
    this.cb = cb;
    console.log('getWord::this.inputIt=<',this.inputIt,'>');
    if(!this.inputIt) {
      return 'prepare';
    } else {
      return 'ready';
    }
    //let end = this.inputIt.end();
    //console.log('getWord::end=<',end,'>');
    //let next = this.inputIt.next();
  }
}

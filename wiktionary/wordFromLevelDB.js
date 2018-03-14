let LevelDown = require('leveldown');
var levelUp = require('levelup');

module.exports = class WordDBUtility {
  constructor(option) {
    if(option.input) {
      console.log('constructor::option.input=<',option.input,'>');
      this.input = levelUp(LevelDown(option.input));
      console.log('constructor::this.input=<',this.input,'>');
    }
    if(option.output) {
      console.log('constructor::option.output=<',option.output,'>');
      this.output = levelUp(option.output);
    }
    this.dry = false;
    if(option.dry) {
      this.dry = option.dry;
    }
    console.log('constructor::this.input=<',this.input,'>');
    console.log('constructor::this.output=<',this.output,'>');
  }
    
  getWord(cb){
    this.cb = cb;
    console.log('readKana_::kana1=<',kana1,'>');
  }
}

let levelup = require('levelup');
module.exports = class WordDBUtility {
  constructor(option) {
    if(option.input) {
      this.input = levelup(option.input);
    }
    if(option.output) {
      this.output = levelup(option.output);
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

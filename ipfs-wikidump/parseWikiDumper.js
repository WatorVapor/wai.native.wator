const fs = require('fs')
const cheerio = require('cheerio');
const readline = require('readline');

module.exports = class WikiDumper {
  constructor(path,onPage) {
    this.path = path;
    this.onPage = onPage;
    this.stream = fs.createReadStream(path, "utf8");
    this.rl = readline.createInterface({'input': this.stream, 'output': {}});
    this.readLines_();
  }
  
  readLines_() {
    let pageStart = false;
    let titlePage = '';
    let self = this;
    this.rl.on('line', function (line) {
      if(line.includes('<page>')) {
        pageStart = true; 
      }
      if(pageStart) {
        titlePage += line;
        titlePage += '\n';
      }
      if(line.includes('</page>')) {
        pageStart = false;
        self.parsePage_(titlePage);
        titlePage = '';
      }
    });
  }

  parsePage_(page) {
    //console.log('parsePage:page =<',page,'>');
    const $ = cheerio.load(page);
    let title = this.fetchTitle_($);
    let text = this.fetchText_($);
    if(typeof this.onPage === 'function') {
      this.onPage(title,text);
    }
  }
  fetchTitle_($) {
    $('title').each( (i, elem) => {
      //console.log('parsePage:elem=<',elem,'>');
      let textElem = elem.children[0];
      if(textElem) {
        //console.log('parsePage:textElem=<',textElem,'>');
        if(textElem.type === 'text') {
          let length = textElem.data.length;
          let textPure = textElem.data.substr(0,length);
          return textPure;
        }
      }
    });
  }
  
  fetchText_($) {
    //console.log('fetchHiraKana:title=<',title,'>');  
    $('text').each( (i, elem) => {
      //console.log('fetchHiraKana:elem=<',elem,'>');
      let textElem = elem.children[0];
      if(textElem) {
        //console.log('fetchHiraKana:textElem=<',textElem,'>');
        if(textElem.type === 'text') {
          let length = textElem.data.length;
          let textPure = textElem.data.substr(0,length);
          return textPure;
        }
      }
    });
  }
  
}

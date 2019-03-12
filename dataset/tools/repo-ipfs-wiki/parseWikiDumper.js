const fs = require('fs')
const cheerio = require('cheerio');
const readline = require('readline');

const ShowProgreeCountSize = 1024*1024;
module.exports = class WikiDumper {
  constructor(path,start,onPage) {
    this.path = path;
    this.onPage = onPage;
    let option = {encoding:'utf-8',start:start,end:start + 1024*1024*128};
    this.stream = fs.createReadStream(path, option);
    this.rl = readline.createInterface({'input': this.stream, 'output': {}});
    this.readLines_();
    const stats = fs.statSync(path);
    this.totalSize = stats.size;
    this.pos = start;
    this.showCounter = 0;
    this.showCounterPre = -1;
  }
  resume() {
    //console.log('resume:this.pos =<',this.pos,'>');
    if(this.stream.isPaused()) {
      this.stream.resume();
    }
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
    this.pos += page.length;
    //console.log('parsePage:this.pos =<',this.pos,'>');
    //console.log('parsePage:ShowProgreeSize =<',ShowProgreeSize,'>');
    //console.log('parsePage:this.showCounter =<',this.showCounter,'>');
    //console.log('parsePage:this.showCounterPre =<',this.showCounterPre,'>');
    this.showCounter = parseInt(this.pos / ShowProgreeCountSize);
    if(this.showCounter !== this.showCounterPre) {
      let percent = 100 * this.pos / this.totalSize;
      console.log('parsePage:this.pos =<',this.pos,'>');
      console.log('parsePage:this.totalSize =<',this.totalSize,'>');
      console.log('parsePage:percent =<',percent,'%>');
      this.showCounterPre = this.showCounter;
    }
    //console.log('parsePage:page =<',page,'>');
    const $ = cheerio.load(page);
    let title = this.fetchTitle_($);
    let text = this.fetchText_($);
    if(typeof this.onPage === 'function') {
      this.stream.pause();
      this.rl.pause();
      this.onPage(title,this.pos,text);
    }
  }
  fetchTitle_($) {
    let title ='';
    $('title').each( (i, elem) => {
      //console.log('fetchTitle_:elem=<',elem,'>');
      let textElem = elem.children[0];
      if(textElem) {
        //console.log('fetchTitle_:textElem=<',textElem,'>');
        if(textElem.type === 'text') {
          let length = textElem.data.length;
          let textPure = textElem.data.substr(0,length);
          //console.log('fetchTitle_:textPure=<',textPure,'>');
          title = textPure;
        }
      }
    });
    return title;
  }
  
  fetchText_($) {
    let text ='';
    $('text').each( (i, elem) => {
      //console.log('fetchText_:elem=<',elem,'>');
      let textElem = elem.children[0];
      if(textElem) {
        //console.log('fetchText_:textElem=<',textElem,'>');
        if(textElem.type === 'text') {
          let length = textElem.data.length;
          let textPure = textElem.data.substr(0,length);
          //console.log('fetchText_:textPure=<',textPure,'>');
          text = textPure;
        }
      }
    });
    return text;
  }
  
}

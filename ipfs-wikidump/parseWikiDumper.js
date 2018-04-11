const fs = require('fs')
const cheerio = require('cheerio');
const readline = require('readline');

const ShowProgreeSize = 10 * 1024 * 1024;
module.exports = class WikiDumper {
  constructor(path,onPage) {
    this.path = path;
    this.onPage = onPage;
    this.stream = fs.createReadStream(path, "utf8");
    this.rl = readline.createInterface({'input': this.stream, 'output': {}});
    this.readLines_();
    const stats = fs.statSync(path);
    this.totalSize = stats.size;
    this.pos = 0;
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
    if(this.pos % ShowProgreeSize === 0) {
      let percent = 100 * this.pos / this.totalSize;
      console.log('parsePage:this.pos =<',this.pos,'>');
      console.log('parsePage:this.totalSize =<',this.totalSize,'>');
      console.log('parsePage:percent =<',percent,'%>');
    }
    //console.log('parsePage:page =<',page,'>');
    const $ = cheerio.load(page);
    let title = this.fetchTitle_($);
    let text = this.fetchText_($);
    if(typeof this.onPage === 'function') {
      this.onPage(title,text);
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

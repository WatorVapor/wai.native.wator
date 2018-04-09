const fs = require('fs')
const cheerio = require('cheerio');
const readline = require('readline');
let stream = fs.createReadStream("./jawiktionary-latest-pages-articles.xml", "utf8");
console.log('stream =<',stream,'>');

let rl = readline.createInterface({'input': stream, 'output': {}});

let pageStart = false;
let titlePage = '';
rl.on('line', function (line) {
  if(line.includes('<page>')) {
    pageStart = true; 
  }
  if(pageStart) {
    titlePage += line;
    titlePage += '\n';
  }
  if(line.includes('</page>')) {
    pageStart = false;
    parsePage(titlePage);
    titlePage = '';
  }
});


function parsePage(page) {
  //console.log('parsePage:page =<',page,'>');
  const $ = cheerio.load(page);
  $('title').each( (i, elem) => {
    //console.log('parsePage:elem=<',elem,'>');
    let textElem = elem.children[0];
    if(textElem) {
      //console.log('parsePage:textElem=<',textElem,'>');
      if(textElem.type === 'text') {
        let length = textElem.data.length;
        let textPure = textElem.data.substr(0,length);
        //console.log('parsePage:textPure=<',textPure,'>');  
        fetchHiraKana($,textPure);
      }
    }
  });
}


function fetchHiraKana($,title) {
  //console.log('fetchHiraKana:title=<',title,'>');  
  $('text').each( (i, elem) => {
    //console.log('fetchHiraKana:elem=<',elem,'>');
    let textElem = elem.children[0];
    if(textElem) {
      //console.log('fetchHiraKana:textElem=<',textElem,'>');
      if(textElem.type === 'text') {
        let length = textElem.data.length;
        let textPure = textElem.data.substr(0,length);
        //console.log('fetchHiraKana:textPure=<',textPure,'>');
        fetchByDEFAULTSORT(title,textPure);
        //fetchByPronJpn(title,textPure);
      }
    }
  });
}

function fetchByDEFAULTSORT(title,textPure) {
  let parma1 = textPure.split('{{DEFAULTSORT:');
  if(parma1[1]) {
    let param2 = parma1[1].split('}}');
    //console.log('fetchByDEFAULTSORT:param2.length=<',param2.length,'>');
    if(param2.length > 1) {
      let param3 = param2[0].split('{{PAGENAME');
      let hirakana = param3[0];
      console.log('fetchByDEFAULTSORT:title=<',title,'>');
      console.log('fetchByDEFAULTSORT:hirakana=<',hirakana,'>');
    }
  }  
}


function fetchByPronJpn(title,textPure) {
  let parma1 = textPure.split('===={{pron|jpn}}====');
  if(parma1[1]) {
    let param2 = parma1[1].split('\n');
    console.log('fetchByPronJpn:param2=<',param2,'>');
    console.log('fetchByPronJpn:param2.length=<',param2.length,'>');
    if(param2.length > 1) {
      let param3 = param2[0].split('');
      let hirakana = param3[0];
      console.log('fetchByPronJpn:title=<',title,'>');
      console.log('fetchByPronJpn:hirakana=<',hirakana,'>');
    }
  }  
}


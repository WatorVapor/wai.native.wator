const wiki = require('./parseWikiDumper.js');
let dumpPath = '/watorvapor/wai.storage/dumps.wikimedia.org/jawiktionary/jawiktionary-20180401-pages-articles.xml';
let wikiDumper = new wiki(dumpPath,onPage);

function onPage(title,text){
  //console.log('onPage::title=<',title,'>');
  //console.log('onPage::text=<',text,'>');
  fetchByDEFAULTSORT(title,text);
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

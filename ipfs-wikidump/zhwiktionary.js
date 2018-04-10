const wiki = require('./parseWikiDumper.js');
let dumpPath = '/watorvapor/wai.storage/dumps.wikimedia.org/zhwiktionary/zhwiktionary-20180401-pages-articles.xml';
let wikiDumper = new wiki(dumpPath,onPage);

function onPage(title,text){
  //console.log('onPage::title=<',title,'>');
  //console.log('onPage::text=<',text,'>');
  fetchByPronPinYin(title,text);
}
function fetchByPronPinYin(title,textPure) {
  //console.log('fetchByPronPinYin::title=<',title,'>');
  let parma1 = textPure.split('{{漢語讀音');
  if(parma1.length > 1) {
    //console.log('fetchByPronPinYin::parma1[1]=<',parma1[1],'>');
    let parma2 = parma1[1].split('}}');
    if(parma2.length > 1) {
      //console.log('fetchByPronPinYin::parma2[0]=<',parma2[0],'>');
      let parma3 = parma2[0].split('漢語拼音=')
      if(parma3.length > 1) {
        //console.log('fetchByPronPinYin::parma3[1]=<',parma3[1],'>');
        let param5 = parma3[1].split('|');
        let pinYin1 = param5[0];
        console.log('fetchByPronPinYin::title=<',title,'>');
        console.log('fetchByPronPinYin::pinYin1=<',pinYin1,'>');
      } else {
        let parma4 = parma2[0].split('{{國音|')
        if(parma4.length > 1) {
          let param6 = parma4[1].split('|');
          let max = param6.length;
          if(max > title.length) {
            max = title.length;
          }
          let pinYin2 ='';
          for(let i = 0 ;i < max;i++) {
            pinYin2 += param6[i];
          }
          console.log('fetchByPronPinYin::title=<',title,'>');
          console.log('fetchByPronPinYin::pinYin2=<',pinYin2,'>');
          console.log('fetchByPronPinYin::title.length=<',title.length,'>');
          //console.log('fetchByPronPinYin::parma3=<',parma3,'>');
        }
      }
    }
  } else {
    //console.log('fetchByPronPinYin::textPure=<',textPure,'>');
  }
}

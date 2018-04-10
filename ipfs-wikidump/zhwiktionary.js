const wiki = require('./parseWikiDumper.js');
let dumpPath = '/watorvapor/wai.storage/dumps.wikimedia.org/zhwiktionary/zhwiktionary-20180401-pages-articles.xml';
let wikiDumper = new wiki(dumpPath,onPage);

function onPage(title,text){
  //console.log('onPage::title=<',title,'>');
  //console.log('onPage::text=<',text,'>');
  fetchByPronPinYin(title,text);
}


function fetchByPronPinYin(title,textPure) {
  if(isAllHanzi(title) == false) {
    return;
  }
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
          //console.log('fetchByPronPinYin::title.length=<',title.length,'>');
          //console.log('fetchByPronPinYin::parma3=<',parma3,'>');
        }
      }
    }
  } else {
    //console.log('fetchByPronPinYin::title=<',title,'>');
    //console.log('fetchByPronPinYin::textPure=<',textPure,'>');
    let parma10 = textPure.split('|pinyin=');
    if(parma10.length > 1) {
      let parame11 = parma10[1].split('|');
      let pinYin3 = parame11[0];
      console.log('fetchByPronPinYin::title=<',title,'>');
      console.log('fetchByPronPinYin::pinYin3=<',pinYin3,'>');      
    } else {
      let parma20 = textPure.split('{{zh-pron');
      if(parma20.length > 1) {
        let parma21 = parma20[1].split('}}');
        let pinYin4 = parma21[0];
        console.log('fetchByPronPinYin::title=<',title,'>');
        console.log('fetchByPronPinYin::pinYin4=<',pinYin4,'>');      
      } else {
        console.log('fetchByPronPinYin::title=<',title,'>');
        console.log('fetchByPronPinYin::textPure=<',textPure,'>');
      }
    }
  }
}


function includeHanzi(utf8Str) {
  //console.log('includeHanzi::utf8Str=<',utf8Str,'>');
  for(let i = 0;i < utf8Str.length;i++) {
    //console.log('includeHanzi::typeof utf8Str[i]=<',typeof utf8Str[i],'>');
    //console.log('includeHanzi::utf8Str[i]=<',utf8Str[i],'>');
    let hanzi = utf8Str[i];
    for(let j = 0;j < HanziRange.length;j++) {
      if(hanzi >= HanziRange[j].b && hanzi <= HanziRange[j].e) {
        return true;
      }
    }
  }
  return false;
}

function isAllHanzi(utf8Str) {
  //console.log('includeHanzi::utf8Str=<',utf8Str,'>');
  for(let i = 0;i < utf8Str.length;i++) {
    //console.log('includeHanzi::typeof utf8Str[i]=<',typeof utf8Str[i],'>');
    //console.log('includeHanzi::utf8Str[i]=<',utf8Str[i],'>');
    let hanzi = utf8Str[i];
    let isHanzi = false;
    for(let j = 0;j < HanziRange.length;j++) {
      if(hanzi >= HanziRange[j].b && hanzi <= HanziRange[j].e) {
        isHanzi = true;
        break;
      }
    }
    if(!isHanzi) {
      return false;
    }
  }
  return true;
}



const HanziRange = [ 
  {b:'㐀',e:'䶵'},
  {b:'一',e:'龥'},
  {b:'龦',e:'鿋'},
  {b:'𠀀',e:'𪛖'},
  {b:'𪜀',e:'𫜴'},
  {b:'𫝀',e:'𫠝'},

  {b:'⼀',e:'⿕'},
  {b:'⺀',e:'⻳'},
  {b:'豈',e:'龎'},
  {b:'丽',e:'鼻'},/* +1 */

  {b:'㇀',e:'㇣'},
  {b:'⿰',e:'⿻'},
  {b:'ㄅ',e:'ㄠ'},
  {b:'ㆠ',e:'ㆷ'},

  {b:'〇',e:'〇'},
  
];


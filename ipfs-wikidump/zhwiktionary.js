const wiki = require('./parseWikiDumper.js');
let dumpPath = '/watorvapor/wai.storage/dumps.wikimedia.org/zhwiktionary/zhwiktionary-20180420-pages-articles.xml';
let wikiDumper = new wiki(dumpPath,0,onPage);

const level = require('level');
let dbPath = '/watorvapor/wai.storage/dumps.wikimedia.org/output_leveldb/zhwiktionary';
let db = level(dbPath);


function pushToDB(title,pinyin) {
  console.log('pushToDB::title=<',title,'>');
  console.log('pushToDB::pinyin=<',pinyin,'>');
  db.put(title,pinyin);
}

function onPage(title,pos,text){
  console.log('onPage::title=<',title,'>');
  console.log('onPage::text=<',text,'>');
  fetchByPronPinYin(title,text);
}


function fetchByPronPinYin(title,textPure) {
  if(isAllHanzi(title) == false) {
    return;
  }
  //console.log('fetchByPronPinYin::title=<',title,'>');
  let hint = tryKeyWord(textPure,'{{漢語讀音',function(parma1){
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
        pushToDB(title,pinYin1);
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
          pushToDB(title,pinYin2);
          //console.log('fetchByPronPinYin::title.length=<',title.length,'>');
          //console.log('fetchByPronPinYin::parma3=<',parma3,'>');
        }
      }
    }
  });
  if(hint) {
    return;
  }
  

  hint = tryKeyWord(textPure,'|pinyin=',function(parma20){
    if(parma20.length > 1) {
      let parma21 = parma20[1].split('}}');
      let parma22 = parma21[0].split('|');
      let pinYin3 = parma22[0];
      console.log('fetchByPronPinYin::title=<',title,'>');
      console.log('fetchByPronPinYin::pinYin3=<',pinYin3,'>');      
      pushToDB(title,pinYin3);
    } 
  });
  if(hint) {
    return;
  }
  
  hint = tryKeyWord(textPure,'{{zh-pron',function(parma20){
    if(parma20.length > 1) {
      let parma21 = parma20[1].split('}}');
      let pinYin4 = parma21[0];
      console.log('fetchByPronPinYin::title=<',title,'>');
      console.log('fetchByPronPinYin::pinYin4=<',pinYin4,'>');
      pushToDB(title,pinYin4);
    }
  });
  if(hint) {
    return;
  }
  
  let keyword = ['{{汉语读音','{{漢語發音'];
  hint = tryKeyWord(textPure,keyword,function(parma31){
    //console.log('fetchByPronPinYin::parma31[1]=<',parma31[1],'>');
    let parma32 = parma31[1].split('}}');
    if(parma32.length > 1) {
      //console.log('fetchByPronPinYin::parma32[0]=<',parma32[0],'>');
      let parma33 = parma32[0].split('汉语拼音=')
      if(parma33.length > 1) {
        //console.log('fetchByPronPinYin::parma33[1]=<',parma33[1],'>');
        let param35 = parma33[1].split('|');
        let pinYin5 = param35[0];
        console.log('fetchByPronPinYin::title=<',title,'>');
        console.log('fetchByPronPinYin::pinYin5=<',pinYin5,'>');
        pushToDB(title,pinYin5);
      } else {
        //console.log('fetchByPronPinYin::parma32=<',parma32,'>');
        let parma36 = parma32[0].split('漢語拼音=');
        if(parma36.length > 1) {
          let param35 = parma36[1].split('|');
          let pinYin7 = param35[0];
          console.log('fetchByPronPinYin::title=<',title,'>');
          console.log('fetchByPronPinYin::pinYin7=<',pinYin7,'>');
          pushToDB(title,pinYin7);
        } else {
          //console.log('fetchByPronPinYin::parma32=<',parma32,'>');
          let parma37 = parma32[0].split('{{國音|');
          if(parma37.length > 1) {
            let param38 = parma37[1].split('|');
            let max = param38.length;
            if(max > title.length) {
              max = title.length;
            }
            let pinYin13 = '';
            for(let i = 0;i < max ;i++) {
              pinYin13 += param38[i];
            }
            console.log('fetchByPronPinYin::title=<',title,'>');
            console.log('fetchByPronPinYin::pinYin13=<',pinYin13,'>');
            //console.log('fetchByPronPinYin::param38=<',param38,'>');
            pushToDB(title,pinYin13);
          } else {
            console.log('fetchByPronPinYin::title=<',title,'>');
            console.log('fetchByPronPinYin::parma32=<',parma32,'>');
          }
        }
      }
    }
  });
  if(hint) {
    return;
  }

  hint = tryKeyWord(textPure,'{{字音',function(parma41){
    //console.log('fetchByPronPinYin::parma41[1]=<',parma41[1],'>');
    let parma42 = parma41[1].split('}}');
    if(parma42.length > 1) {
      //console.log('fetchByPronPinYin::parma42[0]=<',parma42[0],'>');
      let parma43 = parma42[0].split('{{國音|')
      if(parma43.length > 1) {
        //console.log('fetchByPronPinYin::parma43[1]=<',parma43[1],'>');
        let param45 = parma43[1].split('|');
        let pinYin6 = param45[0];
        console.log('fetchByPronPinYin::title=<',title,'>');
        console.log('fetchByPronPinYin::pinYin6=<',pinYin6,'>');
        pushToDB(title,pinYin6);
      } else {
        console.log('fetchByPronPinYin::parma42=<',parma42,'>');
      }
    }
  });
  if(hint) {
    return;
  }

  hint = tryKeyWord(textPure,'漢語拼音：',function(parma51){
    //console.log('fetchByPronPinYin::parma51[1]=<',parma51[1],'>');
    let parma52 = parma51[1].split('\n');
    if(parma52.length > 1) {
      let pinYin26 = parma52[0];
      console.log('fetchByPronPinYin::title=<',title,'>');
      console.log('fetchByPronPinYin::pinYin6=<',pinYin26,'>');
      pushToDB(title,pinYin26);
    } else {
      console.log('fetchByPronPinYin::parma52=<',parma52,'>');
    }
  });
  if(hint) {
    return;
  }

  hint = tryKeyWord(textPure,'[[Wiktionary:汉语拼音索引|汉语拼音]]：',function(parma61){
    //console.log('fetchByPronPinYin::parma61[1]=<',parma61[1],'>');
    let parma62 = parma61[1].split('\n');
    if(parma62.length > 1) {
      let pinYin8 = parma62[0];
      console.log('fetchByPronPinYin::title=<',title,'>');
      console.log('fetchByPronPinYin::pinYin8=<',pinYin8,'>');
      pushToDB(title,pinYin8);
    } else {
      console.log('fetchByPronPinYin::title=<',title,'>');
      console.log('fetchByPronPinYin::parma62=<',parma62,'>');
    }
  });
  if(hint) {
    return;
  }

  hint = tryKeyWord(textPure,'汉语拼音]]:',function(parma71){
    //console.log('fetchByPronPinYin::parma71[1]=<',parma71[1],'>');
    let parma72 = parma71[1].split('\n');
    if(parma72.length > 1) {
      let pinYin9 = parma72[0];
      console.log('fetchByPronPinYin::title=<',title,'>');
      console.log('fetchByPronPinYin::pinYin9=<',pinYin9,'>');
      pushToDB(title,pinYin9);
    } else {
      console.log('fetchByPronPinYin::parma72=<',parma72,'>');
    }
  });
  if(hint) {
    return;
  }

  hint = tryKeyWord(textPure,"汉语拼音]]'''：",function(parma81){
    //console.log('fetchByPronPinYin::parma81[1]=<',parma81[1],'>');
    let parma82 = parma81[1].split('\n');
    if(parma82.length > 1) {
      let pinYin10 = parma82[0];
      console.log('fetchByPronPinYin::title=<',title,'>');
      console.log('fetchByPronPinYin::pinYin10=<',pinYin10,'>');
      pushToDB(title,pinYin10);
    } else {
      console.log('fetchByPronPinYin::parma82=<',parma82,'>');
    }
  });
  if(hint) {
    return;
  }

  hint = tryKeyWord(textPure,'拼音:',function(parma91){
    //console.log('fetchByPronPinYin::parma91[1]=<',parma91[1],'>');
    let parma92 = parma91[1].split('\n');
    if(parma92.length > 1) {
      let pinYin11 = parma92[0];
      console.log('fetchByPronPinYin::title=<',title,'>');
      console.log('fetchByPronPinYin::pinYin11=<',pinYin11,'>');
      pushToDB(title,pinYin11);
    } else {
      console.log('fetchByPronPinYin::parma92=<',parma92,'>');
    }
  });
  if(hint) {
    return;
  }

  hint = tryKeyWord(textPure,'| 拼音=',function(parma111){
    //console.log('fetchByPronPinYin::parma111[1]=<',parma111[1],'>');
    let parma112 = parma111[1].split('\n');
    if(parma112.length > 1) {
      let pinYin12 = parma112[0];
      console.log('fetchByPronPinYin::title=<',title,'>');
      console.log('fetchByPronPinYin::pinYin12=<',pinYin12,'>');
      pushToDB(title,pinYin12);
    } else {
      console.log('fetchByPronPinYin::parma112=<',parma112,'>');
    }
  });
  if(hint) {
    return;
  }
 
  hint = tryKeyWord(textPure,'汉语拼音]]：',function(parma121){
    //console.log('fetchByPronPinYin::parma121[1]=<',parma121[1],'>');
    let parma122 = parma121[1].split('\n');
    if(parma122.length > 1) {
      let pinYin14 = parma122[0];
      console.log('fetchByPronPinYin::title=<',title,'>');
      console.log('fetchByPronPinYin::pinYin14=<',pinYin14,'>');
      //console.log('fetchByPronPinYin::parma122=<',parma122,'>');
      pushToDB(title,pinYin14);
    } else {
      console.log('fetchByPronPinYin::parma121=<',parma121,'>');
    }
  });
  if(hint) {
    return;
  }

  hint = tryKeyWord(textPure,'==拼音==',function(parma131){
    //console.log('fetchByPronPinYin::parma131[1]=<',parma131[1],'>');
    let parma132 = parma131[1].split('\n');
    if(parma132.length > 1) {
      let pinYin15 = parma132[1];
      console.log('fetchByPronPinYin::title=<',title,'>');
      console.log('fetchByPronPinYin::pinYin15=<',pinYin15,'>');
      pushToDB(title,pinYin15);
    } else {
      console.log('fetchByPronPinYin::parma132=<',parma132,'>');
    }
  });
  if(hint) {
    return;
  }
  
  hint = tryKeyWord(textPure,'拼音读音:',function(parma141){
    //console.log('fetchByPronPinYin::parma141[1]=<',parma141[1],'>');
    let parma142 = parma141[1].split('\n');
    if(parma142.length > 1) {
      let pinYin16 = parma142[0];
      console.log('fetchByPronPinYin::title=<',title,'>');
      console.log('fetchByPronPinYin::pinYin16=<',pinYin16,'>');
      pushToDB(title,pinYin16);
    } else {
      console.log('fetchByPronPinYin::parma142=<',parma142,'>');
    }
  });
  if(hint) {
    return;
  }

  hint = tryKeyWord(textPure,'汉语拼音：',function(parma151){
    //console.log('fetchByPronPinYin::parma151[1]=<',parma151[1],'>');
    let parma152 = parma151[1].split('\n');
    if(parma152.length > 1) {
      let pinYin17 = parma152[0];
      console.log('fetchByPronPinYin::title=<',title,'>');
      console.log('fetchByPronPinYin::pinYin17=<',pinYin17,'>');
      pushToDB(title,pinYin17);
    } else {
      console.log('fetchByPronPinYin::parma142=<',parma152,'>');
    }
  });
  if(hint) {
    return;
  }

  
  //console.log('fetchByPronPinYin::title=<',title,'>');
  //console.log('fetchByPronPinYin::textPure=<',textPure,'>');
  
} //fetchByPronPinYin






function tryKeyWord(text,Keyword,cb) {
  //console.log('tryKeyWord::text=<',text,'>');
  if(typeof Keyword === 'string') {
    let parma = text.split(Keyword);
    if(parma.length > 1) {
      cb(parma);
      return true;
    }
  }
  if(typeof Keyword === 'object') {
    for(let i = 0;i < Keyword.length;i++) {
      let kw = Keyword[i];
      let parma = text.split(kw);
      if(parma.length > 1) {
        cb(parma);
        return true;
      }
    }
  }
  return false;
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


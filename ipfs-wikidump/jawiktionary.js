const wiki = require('./parseWikiDumper.js');
let dumpPath = '/watorvapor/wai.storage/dumps.wikimedia.org/jawiktionary/jawiktionary-20180401-pages-articles.xml';
let wikiDumper = new wiki(dumpPath,onPage);

const level = require('level');
let dbPath = '/watorvapor/wai.storage/dumps.wikimedia.org/output_leveldb/jawiktionary';
let db = level(dbPath);

function pushToDB(title,hirakana) {
  console.log('pushToDB::title=<',title,'>');
  console.log('pushToDB::hirakana=<',hirakana,'>');
  db.put(title,hirakana);
}


function onPage(title,textPure){
  //console.log('onPage::title=<',title,'>');
  //console.log('onPage::text=<',textPure,'>');
  //fetchByDEFAULTSORT(title,textPure);
  let filters = [
    'MediaWiki:','Wiktionary:','テンプレート:',
    'メインページ','ヘルプ:','ファイル:',
    'カテゴリ:','付録:'
  ];
  if(filterTitle(filters,title)) {
    //console.log('onPage::filter out title=<',title,'>');
    return;
  }
  if(filterAccii(title)) {
    //console.log('onPage::filter out title=<',title,'>');
    return;
  }
  fetchKana(title,textPure);
}

function fetchKana(title,textPure) {
  let hint = false;
  hint = tryKeyWord(textPure,'{{DEFAULTSORT:',function(parma1){
    let param2 = parma1[1].split('}}');
    if(param2.length > 1) {
      let param3 = param2[0].split('{{PAGENAME');
      let hirakana1 = param3[0];
      console.log('fetchKana:title=<',title,'>');
      console.log('fetchKana:hirakana1=<',hirakana1,'>');
      //pushToDB(title,hirakana1);
    }
  });
  if(hint) {
    return;
  }
  let categoryJa = [
    '[[Category:{{ja}}|',
    '[[category:{{ja}}|',
    '[[Category:{{ja}}',
    '[[category:{{ja}}',
    '[[Category:{{jpn}}|',
    '[[category:{{jpn}}|',
    '[[Category:{{jpn}}',
    '[[Category:日本語|',
    '[[Category:日本語',
    '[[category:日本語|',
    '[[category:日本語',
    '[[カテゴリ:{{jpn}}|',
    '[[カテゴリ:{{jpn}}',
    '[[Category:漢字|',
    '[[Category:約物|',
    '[[Category:Unicode_CJK_Compatibility|',
  ];

  hint = tryKeyWord(textPure,categoryJa,function(parma1){
    let param2 = parma1[1].split(']]');
    if(param2.length > 1) {
      let hirakana2 = param2[0];
      console.log('fetchKana:title=<',title,'>');
      console.log('fetchKana:hirakana2=<',hirakana2,'>');
      //pushToDB(title,hirakana2);
    } else {
      console.log('fetchKana:title=<',title,'>');
      console.log('fetchKana:param2=<',param2,'>');
    }
  });
  if(hint) {
    return;
  }

  
  let keyswords = [
    '==={{pron|ja}}===',
    '===={{pron|jpn}}====',
    '=== 読み ==='
  ];
  hint = tryKeyWord(textPure,keyswords,function(parma1){
    let param2 = parma1[1].split('===');
    if(param2.length > 1) {
      let param3 = param2[0].split('{{PAGENAME');
      let hirakana3 = param3[0];
      console.log('fetchKana:title=<',title,'>');
      console.log('fetchKana:hirakana3=<',hirakana3,'>');
      //pushToDB(title,hirakana3);
    } else {
      console.log('fetchKana:title=<',title,'>');
      console.log('fetchKana:parma1[1]=<',parma1[1],'>');
    }
  });
  if(hint) {
    return;
  }
  
  let category = [
    '[[Category:かな]]',        '[[category:かな]]',
    '[[カテゴリ:かな]]',         '[[Category:かな|',
    '[[Category:{{kor}}]]',     '[[category:{{kor}}]]',
    '=={{kor}}==',
    
    '[[category:{{zh}}',        '[[Category:中国語',
    '[[Category:{{zho}}',       '[[Category:{{zh}}',
    '[[Category:{{zh}}',
/*
    '[[Category:{{en}}]]',      '[[category:{{en}}]]',
    '[[Category:{{pl}}]]',      '[[category:{{pl}}]]',
    '[[category:{{fr}}]]',
    '[[category:{{it}}]]',      '[[Category:{{it}}]]',
    '[[Category:Unicode Basic Latin]]' ,'[[category:Unicode Basic Latin]]'
*/
  ];
  hint = tryKeyWord(textPure,category,function(parma1){
    console.log('fetchKana:title=<',title,'>');
  });
  if(hint) {
    return;
  }
  console.log('fetchKana::title=<',title,'>');
  console.log('fetchKana::textPure=<',textPure,'>');
  
}
 

function filterTitle(filters,title) {
  for(let i = 0;i < filters.length;i++) {
    let filter = filters[i];
    if(title.includes(filter)) {
      return true;
    }
  }
  return false;
}

function filterAccii(title) {
  for(let i = 0;i < title.length;i++) {
    let charCode = title.charAt(i);
    //console.log('filterAccii:charCode=<',charCode,'>');
    if(charCode >= '、') {
      return false;
    }
  }
  return true;
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
      pushToDB(title,hirakana);
    } else {
      console.log('fetchByDEFAULTSORT:title=<',title,'>');
      console.log('fetchByDEFAULTSORT:param2=<',param2,'>');
    }
  } else {
    console.log('fetchByDEFAULTSORT:title=<',title,'>');
    console.log('fetchByDEFAULTSORT:parma1=<',parma1,'>');
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


function tryKeyWord(text,Keyword,cb) {
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


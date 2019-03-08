$(document).ready(() => {
  //console.log('onWaiResource CJK_Table=<',CJK_Table,'>');
  onInitIpfs();
});


onInitIpfs = ()=> {
  const node = new window.Ipfs();
  node.on('ready', () => {
    //console.log('onInitIpfs node=<',node,'>');
    showIpfsInfo(node);
    doReadFile(node);
  })
}

showIpfsInfo = (node) => {
  node.id(function (err, identity) {
    if (err) {
      throw err;
    }
    console.log('showIpfsInfo identity=<',identity,'>');
  })  
}

const validCID = 'QmfZmje1L9tq4RMUHM7Y3sk3Zohrm9UQarujHyENUhR8cJ';
doReadFile = (node) => {
  node.get(validCID, (err, files) => {
    if(err) {
      throw err;
    }
    files.forEach((file) => {
      //console.log('doReadFile file.path=<',file.path,'>');
      //console.log('doReadFile file.content=<',file.content.toString('utf8'),'>');
      onWaiDocument(file.content.toString('utf8'));
    })
  })  
};


onWaiDocument = (article) => {
  let aDocumentStatistics = {};
  //console.log('onWaiDocument article=<',article,'>');
  let cjkCollect = [];
  let cjkBuffer = [];
  for(let i = 0 ;i < article.length;i++) {
    let utf8 = article[i];
    //console.log('onWaiDocument utf8=<',utf8,'>');
    let isCJK = CJK_Table[utf8];
    //console.log('onWaiDocument isCJK=<',isCJK,'>');
    if(isCJK) {
      cjkBuffer.push(utf8);
    } else {
      if(cjkBuffer.length > 0) {
        cjkCollect.push(cjkBuffer);
      }
      cjkBuffer = [];
    }
  }
  if(cjkBuffer.length > 0) {
    cjkCollect.push(cjkBuffer);
  }  
  //console.log('onWaiDocument cjkCollect=<',cjkCollect,'>');
  for(let i = 0 ;i < cjkCollect.length;i++) {
    onWaiSentence(cjkCollect[i],aDocumentStatistics);
  }
  //console.log('onWaiDocument aDocumentStatistics=<',aDocumentStatistics,'>');
  FilterOutLowFreq(aDocumentStatistics);
  console.log('onWaiDocument aDocumentStatistics=<',aDocumentStatistics,'>');
  FilterOutInside(aDocumentStatistics);
  console.log('onWaiDocument aDocumentStatistics=<',aDocumentStatistics,'>');
}

const iConstNGramMaxWindow = 32;

onWaiSentence = (sentence,collect) => {
  //console.log('onWaiSentence sentence=<',sentence,'>');
  for(let i = 0 ;i < sentence.length;i++) {
    let utf8 = sentence[i];
    //console.log('onWaiSentence utf8=<',utf8,'>');
    let backIndex = i;
    if(backIndex > iConstNGramMaxWindow) {
      backIndex = iConstNGramMaxWindow;
    }
    for(let j = 1 ;j <= backIndex;j++) {
      let start = i - j ;
      if(start >= 0) {
        let concat = sentence.slice(start,i+1);
        let word = concat.join('');
        //console.log('onWaiSentence word=<',word,'>');
        if(collect[word]) {
          collect[word]++;
        } else {
          collect[word] = 1;
        }
      }
    }
  }
}

const iConstWordFilterOutStageOne = 3;


FilterOutLowFreq = (collect) => {
  let keys = Object.keys(collect);
  for(let i = 0 ;i < keys.length;i++) {
    let key = keys[i];
    if(collect[key] < iConstWordFilterOutStageOne) {
      delete collect[key];
    }
  }
}

FilterOutInside = (collect) => {
  let keys = Object.keys(collect);
  for(let i = 0 ;i < keys.length;i++) {
    let key = keys[i];
    for(let j = 0 ;j < keys.length;j++) {
      let keyFound = keys[j];
      if(keyFound.includes(key)) {
        if(collect[keyFound] === collect[key]) {
          delete collect[key];
        }
      }
    }
  }
}

isInsideOtherWord = (key,keys,collect) => {
  for(let i = 0 ;i < keys.length;i++) {
    let keyFound = keys[i];
    if(keyFound.includes(key)) {
      
    }
  }
}

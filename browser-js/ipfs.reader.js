$(document).ready(() => {
  //console.log('onWaiResource CJK_Table=<',CJK_Table,'>');
  onInitIpfs();
});


onInitIpfs = ()=> {
  const node = new window.Ipfs();
  node.on('ready', () => {
    //console.log('onInitIpfs node=<',node,'>');
    doReadFile(node);
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
      onWaiResource(file.content.toString('utf8'));
    })
  })  
};

onWaiResource = (text) => {
  //console.log('onWaiResource text=<',text,'>');
  let cjkCollect = [];
  let cjkBuffer = [];
  for(let i = 0 ;i < text.length;i++) {
    let utf8 = text[i];
    //console.log('onWaiResource utf8=<',utf8,'>');
    let isCJK = CJK_Table[utf8];
    //console.log('onWaiResource isCJK=<',isCJK,'>');
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
  console.log('onWaiResource cjkCollect=<',cjkCollect,'>');
}

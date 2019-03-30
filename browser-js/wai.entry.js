$(document).ready(() => {
  //console.log('onWaiResource CJK_Table=<',CJK_Table,'>');
  onWaiInit();
});

onWaiInit = () => {
  let parrot = new WaiParrot();
  console.log('onWaiInit parrot=<',parrot,'>');
  let crystal = new WaiCrystal(parrot);
  console.log('onWaiInit crystal=<',crystal,'>');
  
  crystal.entryBlock = () => {
    parrot.entryBlock();
  }
  crystal.onArticle = (doc) => {
    parrot.article(doc);
  }
  crystal.leaveBlock = () => {
    parrot.leaveBlock();
  }
}

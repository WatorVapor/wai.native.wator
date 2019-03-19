$(document).ready(() => {
  //console.log('onWaiResource CJK_Table=<',CJK_Table,'>');
  onWaiInit();
});

onWaiInit = () => {
  let crystal = new WaiCrystal();
  console.log('onWaiInit crystal=<',crystal,'>');
  let parrot = new WaiParrot();
  console.log('onWaiInit parrot=<',parrot,'>');
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

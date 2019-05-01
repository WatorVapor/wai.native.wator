let blockInfoApp = false;

onUpdateVueUI = (block) => {
  console.log('onUpdateVueUI block=<',block,'>');
  const percentage = Math.floor(100*block.step/block.total);
  console.log('onUpdateVueUI percentage=<',percentage,'>');
  if(blockInfoApp) {
    blockInfoApp.block = block;
    blockInfoApp.percentage = percentage;
  } else {
    blockInfoApp = new Vue({
      el: '#ui-vue-mining-block',
      data: {
        block: block,
        percentage:percentage
      }
    });    
  }
}

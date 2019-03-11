const wiki = require('./parseWikiDumper.js');
const leveldown = require('leveldown');
const IpfsSave = require('./ipfs-save.js');
const LevelSave = require('./ipfs-level.js');
const opencc = require('node-opencc');

module.exports = class IpfsMain {
  constructor(dumpPath,dbPath,skipTitles) {
    this.dumpPath = dumpPath;
    this.dbPath = dbPath;
    this.skipTitles = skipTitles;
    this.wikiDumper = false;
    this.iSave = new IpfsSave();
    this.lSave = false;
    let self = this;
    this.iSave.onReady = (err) => {
      if(err) {
        throw err;
      }
      self.lSave = new LevelSave(self.dbPath,(ResumePos)=> {
        console.log('IpfsMain ::ResumePos=<',ResumePos,'>');
        self.wikiDumper = new wiki(self.dumpPath,ResumePos,self._onPage.bind(self));
      });
    }
  }
  _onPage(zhTitle,pos,zhText) {
    //console.log('_onPage::zhTitle=<',zhTitle,'>');
    if(!zhTitle) {
      if(this.wikiDumper) {
        this.wikiDumper.resume();
      }
      return;
    }
    if(this._filterTitle(this.skipTitles,zhTitle)) {
      //console.log('_onPage::filter out zhTitle=<',zhTitle,'>');
      if(this.wikiDumper) {
        this.wikiDumper.resume();
      }
      return;
    }
    //console.log('_onPage::zhTitle=<',zhTitle,'>');
    let cnTitle = opencc.traditionalToSimplified(zhTitle);
    let cnText = opencc.traditionalToSimplified(zhText);
    //console.log('_onPage::cnTitle=<',cnTitle,'>');
    let self = this;
    this.iSave.save(cnTitle,cnText,pos,(hash,resume) => {
      //console.log('_onPage::hash=<',hash,'>');
      self.lSave.save(hash,cnTitle,pos,()=> {
        //console.log('_onPage::resume=<',resume,'>');
        if(resume) {
          self.wikiDumper.resume();
        }
      });
    });
  }
  
  _filterTitle(filters,title) {
    for(let i = 0;i < filters.length;i++) {
      let filter = filters[i];
      if(title.includes(filter)) {
        return true;
      }
    }
    return false;
  }
}


const dumpPath = '/watorvapor/wai.storage/dumps.wikimedia.org/zhwiki/zhwiki-20190201-pages-articles.xml';
const dbPath = './output_repo/cnwiki/fs';
const fsPath = './output_repo/cnwiki/repo';
const  skipTitles = [
  'Wikipedia:','Help:','Template:','Category:','MediaWiki:','Hex',
  'File:','Portal:','模块:',
];

const Main = require('./ipfs-main.js');
const main = new Main(dumpPath,dbPath,fsPath,skipTitles);

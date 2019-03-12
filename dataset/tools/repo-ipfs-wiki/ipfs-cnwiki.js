const dumpPath = '/watorvapor/wai.storage/dumps.wikimedia.org/zhwiki/zhwiki-20190201-pages-articles.xml';
const dbPath = '/watorvapor/wai.storage/dumps.wikimedia.org/output_repo/cnwiki/fs';
const fsPath = '/watorvapor/wai.storage/dumps.wikimedia.org/output_repo/cnwiki/repo';
const  skipTitles = [
  'Wikipedia:','Help:','Template:','Category:','MediaWiki:','Hex',
  'File:','Portal:','模块:',
];

const Main = require('./ipfs-main.js');
const main = new Main(dumpPath,dbPath,fsPath,skipTitles);

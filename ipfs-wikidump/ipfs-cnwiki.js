const level = require('level');
let dbPath = '/watorvapor/wai.storage/dumps.wikimedia.org/output_leveldb/cnwiktionary';
let db = level(dbPath);

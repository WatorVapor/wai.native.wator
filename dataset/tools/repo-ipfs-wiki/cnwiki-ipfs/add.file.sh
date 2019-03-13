#!/bin/bash
#docker logs -f ipfs_cnwiki
docker exec ipfs_cnwiki ipfs add -r /data/export/readme

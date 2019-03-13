#!/bin/bash
wc=/ceph/storage3/ipfs/wai.native.wator/dataset/tools/repo-ipfs-wiki/cnwiki-ipfs

docker run \
  -v ${wc}/export:/data/export \
  -v ${wc}/storage:/data/ipfs \
  -e IPFS_PROFILE=server,badgerds \
  --name ipfs_cnwiki \
  -it ipfs/go-ipfs:release \
   daemon


#  -e IPFS_PROFILE=server,badgerds\


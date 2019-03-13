#!/bin/bash
wc=/ceph/storage3/ipfs/wai.native.wator/dataset/tools/repo-ipfs-wiki/cnwiki-ipfs

docker run \
  -v ${wc}/export:/data/export \
  -v ${wc}/storage:/data/ipfs \
  -it ipfs/go-ipfs:release \
  ipfs init --profile=server

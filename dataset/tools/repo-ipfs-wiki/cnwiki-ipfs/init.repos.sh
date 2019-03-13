#!/bin/bash
docker run \
  -v ./export:/export \
  -v ./storage:/data/ipfs \
  -it ipfs/go-ipfs:release \
  ipfs init --help
  
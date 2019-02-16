#!/bin/bash
Storage=`pwd`/storage-ipfs
export IPFS_PATH=${Storage}
env
go-ipfs/ipfs init

#!/bin/bash
ulimit -c unlimited
nohup object/wator.parrot.app >wator.parrot.app.1.log &
nohup object/wator.parrot.app >wator.parrot.app.2.log &
nohup object/wator.parrot.app >wator.parrot.app.3.log &

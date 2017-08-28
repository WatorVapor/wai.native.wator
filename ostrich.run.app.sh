#!/bin/bash
ulimit -c unlimited
nohup object/wator.ostrich.app >wator.ostrich.app.1.log &
nohup object/wator.ostrich.app >wator.ostrich.app.2.log &
nohup object/wator.ostrich.app >wator.ostrich.app.3.log &

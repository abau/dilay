#!/bin/bash

BRANCH=$(cd /dilay && git symbolic-ref -q HEAD)

cd /tmp
git clone --depth=1 file:///dilay 

cd /tmp/dilay
qmake -r
make release -j$(nproc)

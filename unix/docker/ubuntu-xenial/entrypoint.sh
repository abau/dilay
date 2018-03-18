#!/bin/bash

cd /tmp
git clone --depth=1 file:///dilay 

cd /tmp/dilay
source /opt/qt*/bin/qt*-env.sh
qmake -r
make release -j$(nproc)

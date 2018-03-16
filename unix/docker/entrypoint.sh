#!/bin/bash

cd /tmp
git clone --depth=1 file:///dilay 

cd /tmp/dilay

if [ -f /opt/qt*/bin/qt*-env.sh ]
then
  source /opt/qt*/bin/qt*-env.sh
fi

qmake -r
make release -j$(nproc)

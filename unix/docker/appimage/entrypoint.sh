#!/bin/bash

cd /tmp
git clone --depth=1 file:///dilay 

cd /tmp/dilay
source /opt/qt*/bin/qt*-env.sh
qmake -r PREFIX=/usr
make release -j$(nproc)
make INSTALL_ROOT=appdir install

wget -c -q "https://github.com/probonopd/linuxdeployqt/releases/download/continuous/linuxdeployqt-continuous-x86_64.AppImage"
unset QTDIR
unset QT_PLUGIN_PATH
unset LD_LIBRARY_PATH
export VERSION=$(git describe --dirty --tags) # linuxdeployqt uses this for naming the file
chmod a+x linuxdeployqt*.AppImage
./linuxdeployqt*.AppImage --appimage-extract
./squashfs-root/AppRun ./app/appdir/usr/share/applications/*.desktop -bundle-non-qt-libs
./squashfs-root/AppRun ./app/appdir/usr/share/applications/*.desktop -appimage
mv Dilay*.AppImage /dilay

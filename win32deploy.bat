@echo off

if exist win32deploy ( rmdir /s /q win32deploy )

mkdir win32deploy

xcopy %2                         win32deploy
xcopy %1\bin\libgcc_s_dw2-1.dll  win32deploy
xcopy %1\bin\libstdc++-6.dll     win32deploy
xcopy %1\bin\libwinpthread-1.dll win32deploy
xcopy %1\bin\Qt5Core.dll         win32deploy
xcopy %1\bin\Qt5Gui.dll          win32deploy
xcopy %1\bin\Qt5Widgets.dll      win32deploy
xcopy %1\bin\Qt5Xml.dll          win32deploy
    
mkdir win32deploy\translations

xcopy %1\translations\qtbase_de.qm win32deploy\translations

mkdir win32deploy\platforms

xcopy %1\plugins\platforms\qwindows.dll win32deploy\platforms
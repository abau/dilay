@echo off

if exist win32deploy ( rmdir /s /q win32deploy )

mkdir win32deploy
copy %2 win32deploy

%1\windeployqt.exe --release ^
                   --no-system-d3d-compiler ^
                   --no-webkit ^
                   --no-angle ^
                   --no-svg ^
                   win32deploy
REM Qt's install directory: %1
REM compilation's target directory: %2
REM Dilay's source directory: %3
REM Dilay's version: %4
@echo off

if exist win32deploy ( rmdir /s /q win32deploy )

mkdir win32deploy
mkdir win32deploy\Dilay

xcopy %2\dilay.exe                         win32deploy\Dilay
xcopy %1\bin\libgcc_s_dw2-1.dll            win32deploy\Dilay
xcopy %1\bin\libstdc++-6.dll               win32deploy\Dilay
xcopy %1\bin\libwinpthread-1.dll           win32deploy\Dilay
xcopy %1\bin\Qt5Core.dll                   win32deploy\Dilay
xcopy %1\bin\Qt5Gui.dll                    win32deploy\Dilay
xcopy %1\bin\Qt5Widgets.dll                win32deploy\Dilay
xcopy %1\bin\Qt5Xml.dll                    win32deploy\Dilay
xcopy %3\win32\icon.ico      	           win32deploy\Dilay
xcopy %3\win32\LICENSE.rtf                 win32deploy\Dilay
    
mkdir win32deploy\Dilay\platforms

xcopy %1\plugins\platforms\qwindows.dll win32deploy\Dilay\platforms

"%ProgramFiles(x86)%\WiX Toolset v3.11\bin\candle.exe" -dDilayVersion=%4 -out win32deploy/dilay.wixobj %3\win32\installer.wix
"%ProgramFiles(x86)%\WiX Toolset v3.11\bin\light.exe" -ext WixUIExtension -b win32deploy -cultures:en-US -out win32deploy/dilay-%4.msi win32deploy/dilay.wixobj

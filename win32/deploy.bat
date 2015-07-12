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
xcopy %3\win32-data\icon.ico               win32deploy\Dilay
xcopy %3\win32-data\LICENSE.rtf            win32deploy\Dilay
xcopy %3\win32-data\banner1-compressed.bmp win32deploy\Dilay
xcopy %3\win32-data\banner2-compressed.bmp win32deploy\Dilay
    
mkdir win32deploy\\Dilay\translations

xcopy %2\app\translations\dilay_de.qm  win32deploy\Dilay\translations
xcopy %1\translations\qtbase_de.qm     win32deploy\Dilay\translations

mkdir win32deploy\Dilay\platforms

xcopy %1\plugins\platforms\qwindows.dll win32deploy\Dilay\platforms

"%ProgramFiles(x86)%\WiX Toolset v3.9\bin\candle.exe" -dDilayVersion=%4 -out win32deploy/dilay.wixobj %3\win32\installer.wix
"%ProgramFiles(x86)%\WiX Toolset v3.9\bin\light.exe" -ext WixUIExtension -b win32deploy -cultures:en-US -out win32deploy/dilay-%4-en.msi win32deploy/dilay.wixobj
"%ProgramFiles(x86)%\WiX Toolset v3.9\bin\light.exe" -ext WixUIExtension -b win32deploy -cultures:de-DE -out win32deploy/dilay-%4-de.msi win32deploy/dilay.wixobj
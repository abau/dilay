include (../common.pri)

TEMPLATE        = app
DESTDIR         = $$OUT_PWD/..
DEPENDPATH     += src 
INCLUDEPATH    += src $$PWD/../lib/src
SOURCES        += src/main.cpp

CONFIG(release, debug|release): TARGET = dilay
CONFIG(debug  , debug|release): TARGET = dilay_debug

win32 {
  CONFIG(release, debug|release) {
    LIBS += -L$$OUT_PWD/../lib/release/ -ldilay

    win32-g++ {
      PRE_TARGETDEPS += $$OUT_PWD/../lib/release/libdilay.a
    }
    else {
      PRE_TARGETDEPS += $$OUT_PWD/../lib/release/dilay.lib
    }
  }
  CONFIG(debug, debug|release) {
    LIBS += -L$$OUT_PWD/../lib/debug/ -ldilay

    win32-g++ {
      PRE_TARGETDEPS += $$OUT_PWD/../lib/debug/libdilay.a
    }
    else {
      PRE_TARGETDEPS += $$OUT_PWD/../lib/debug/dilay.lib
    }
  }
  RC_ICONS = $$PWD/../win32/icon.ico

  QMAKE_POST_LINK += $$PWD/../win32/deploy.bat $$shell_path($$[QT_INSTALL_PREFIX]) \
                                               $$shell_path($$DESTDIR) \
                                               $$shell_path($$PWD/../) \
                                               $$VERSION
}

unix {
  LIBS           += -L$$OUT_PWD/../lib/ -ldilay
  PRE_TARGETDEPS += $$OUT_PWD/../lib/libdilay.a

  target.path     = $$PREFIX/bin/
  INSTALLS       += target

  desktop.path    = $$PREFIX/share/applications/
  desktop.files   = ../unix/dilay.desktop
  INSTALLS       += desktop

  icon16.path     = $$PREFIX/share/icons/hicolor/16x16/apps/
  icon16.files    = ../unix/icon/16x16/dilay.png
  INSTALLS       += icon16

  icon32.path     = $$PREFIX/share/icons/hicolor/32x32/apps/
  icon32.files    = ../unix/icon/32x32/dilay.png
  INSTALLS       += icon32

  icon48.path     = $$PREFIX/share/icons/hicolor/48x48/apps/
  icon48.files    = ../unix/icon/48x48/dilay.png
  INSTALLS       += icon48

  icon256.path    = $$PREFIX/share/icons/hicolor/256x256/apps/
  icon256.files   = ../unix/icon/256x256/dilay.png
  INSTALLS       += icon256

  format.commands = clang-format -style=file -i $$SOURCES $$HEADERS
  QMAKE_EXTRA_TARGETS += format
}

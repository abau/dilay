include (../common.pri)
include (../translations.pri)

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
  QMAKE_POST_LINK += $$PWD/../win32deploy.bat $$shell_path($$[QT_INSTALL_BINS]) $$shell_path($$DESTDIR/$${TARGET}.exe)
}

unix {
  LIBS           += -L$$OUT_PWD/../lib/ -ldilay
  PRE_TARGETDEPS += $$OUT_PWD/../lib/libdilay.a

  target.path     = $$PREFIX/bin/
  qm.files        = $$OUT_PWD/translations/
  qm.path         = $$PREFIX/share/$$TARGET
  INSTALLS       += target qm
}

compile_ts.commands  = $$QMAKE_LRELEASE ${QMAKE_FILE_NAME} -qm ${QMAKE_FILE_OUT}
compile_ts.depends   = $$TRANSLATIONS
compile_ts.input     = TRANSLATIONS
compile_ts.output    = $$OUT_PWD/translations/${QMAKE_FILE_BASE}.qm
compile_ts.CONFIG   += no_link target_predeps

QMAKE_EXTRA_COMPILERS += compile_ts

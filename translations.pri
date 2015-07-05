TRANSLATIONS = $$PWD/i18n/dilay_de.ts

win32 {
  QMAKE_LUPDATE  = $$[QT_INSTALL_BINS]/lupdate.exe
  QMAKE_LRELEASE = $$[QT_INSTALL_BINS]/lrelease.exe
}
unix {
  QMAKE_LUPDATE  = $$[QT_INSTALL_BINS]/lupdate
  QMAKE_LRELEASE = $$[QT_INSTALL_BINS]/lrelease
}

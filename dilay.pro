include (translations.pri)

CONFIG      += debug_and_release
TEMPLATE     = subdirs
SUBDIRS      = lib app test

app.depends  = lib
test.depends = lib

unix {
  lupdate.commands  = $$QMAKE_LUPDATE $$PWD/lib/lib.pro -ts $$TRANSLATIONS
  gdb.commands      = gdb -ex run ./dilay_debug
  valgrind.commands = valgrind ./dilay_debug &> valgrind.log
  leak.commands     = valgrind --leak-check=yes ./dilay_debug &> valgrind.log

  QMAKE_EXTRA_TARGETS += lupdate gdb valgrind leak
}

CONFIG      += debug_and_release
TEMPLATE     = subdirs
SUBDIRS      = lib app test

app.depends  = lib
test.depends = lib

unix {
  gdb.commands      = gdb -ex run ./dilay_debug
  valgrind.commands = valgrind ./dilay_debug &> valgrind.log
  leak.commands     = valgrind --leak-check=yes ./dilay_debug &> valgrind.log

  QMAKE_EXTRA_TARGETS += gdb valgrind leak
}

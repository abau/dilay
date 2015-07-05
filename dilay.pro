include (translations.pri)

TEMPLATE     = subdirs
SUBDIRS      = lib app test

app.depends  = lib
test.depends = lib

release.commands  = $(QMAKE) -r CONFIG+=release; $(MAKE)
debug.commands    = $(QMAKE) -r CONFIG+=debug; $(MAKE)
lupdate.commands  = $$QMAKE_LUPDATE $$PWD/dilay.pro -ts $$TRANSLATIONS
gdb.commands      = gdb -ex run ./dilay_debug
valgrind.commands = valgrind ./dilay_debug &> valgrind.log
leak.commands     = valgrind --leak-check=yes ./dilay_debug &> valgrind.log

QMAKE_EXTRA_TARGETS += release debug lupdate gdb valgrind leak

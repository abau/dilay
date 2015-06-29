TRANSLATIONS    = i18n/dilay_de.ts
TRANSLATIONS_QM = $(TRANSLATIONS:.ts=.qm)
MAKEFILE_QT     = Makefile.qt
QMAKE           = qmake -r -o $(MAKEFILE_QT) "PREFIX=$(PREFIX)" "QM_FILES=$(TRANSLATIONS_QM)"

error:
	@echo "Please specify one of the following targets: release, debug, install, clean"
	@exit 1

debug: $(TRANSLATIONS_QM)
	$(QMAKE) -after "CONFIG += debug"
	$(MAKE) -f $(MAKEFILE_QT)

release: $(TRANSLATIONS_QM)
	$(QMAKE) -after "CONFIG += release"
	$(MAKE) -f $(MAKEFILE_QT)

clean:
	$(MAKE) -f $(MAKEFILE_QT) clean
	$(MAKE) -f $(MAKEFILE_QT) distclean
	rm -f $(TRANSLATIONS_QM)

install:
	$(MAKE) -f $(MAKEFILE_QT) install

uninstall:
	$(MAKE) -f $(MAKEFILE_QT) uninstall

lupdate:
	lupdate dilay.pro -ts $(TRANSLATIONS)

i18n/%.qm: i18n/%.ts
	lrelease $< $@

gdb: debug
	gdb -ex run ./dilay

valgrind: debug
	valgrind ./dilay &> valgrind.log

valgrind-leak: debug
	valgrind --leak-check=yes ./dilay &> valgrind.log

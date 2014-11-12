MAKEFILE_QT=Makefile.qt

debug:
	qmake -r -o ${MAKEFILE_QT} "CONFIG += debug"
	make -f ${MAKEFILE_QT} -j4

release:
	qmake -r -o ${MAKEFILE_QT} "CONFIG += release"
	make -f ${MAKEFILE_QT} -j4

clean:
	make -f ${MAKEFILE_QT} clean

gdb:
	gdb -ex run ./dilay

valgrind:
	valgrind ./dilay &> valgrind.log

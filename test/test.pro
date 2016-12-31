include (../common.pri)

TEMPLATE        = app
TARGET          = run-tests
DESTDIR         = $$OUT_PWD/..
DEPENDPATH     += src 
INCLUDEPATH    += src $$PWD/../lib/src

SOURCES += \
           src/main.cpp \
           src/test-bitset.cpp \
           src/test-distance.cpp \
           src/test-intersection.cpp \
           src/test-intrusive-list.cpp \
           src/test-maybe.cpp \
           src/test-misc.cpp \
           src/test-monotone-deque.cpp \
           src/test-octree.cpp \
           src/test-tree.cpp

HEADERS += \
           src/test-bitset.hpp \
           src/test-distance.hpp \
           src/test-intersection.hpp \
           src/test-intrusive-list.hpp \
           src/test-maybe.hpp \
           src/test-misc.hpp \
           src/test-monotone-deque.hpp \
           src/test-octree.hpp \
           src/test-tree.hpp

win32:CONFIG(release, debug|release):    LIBS += -L$$OUT_PWD/../lib/release/ -ldilay
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../lib/debug/ -ldilay
else:unix:                               LIBS += -L$$OUT_PWD/../lib/ -ldilay

win32-g++:CONFIG(release, debug|release):             PRE_TARGETDEPS += $$OUT_PWD/../lib/release/libdilay.a
else:win32-g++:CONFIG(debug, debug|release):          PRE_TARGETDEPS += $$OUT_PWD/../lib/debug/libdilay.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../lib/release/dilay.lib
else:win32:!win32-g++:CONFIG(debug, debug|release):   PRE_TARGETDEPS += $$OUT_PWD/../lib/debug/dilay.lib
else:unix:                                            PRE_TARGETDEPS += $$OUT_PWD/../lib/libdilay.a

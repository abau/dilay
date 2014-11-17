! include( ../common.pri ) {
    error( Can not find .common.pri )
}

TEMPLATE               =  app
TARGET                 =  run-tests
DESTDIR                =  ..
DEPENDPATH             += src 
INCLUDEPATH            += src ../lib/src
LIBS                   += -L../lib -ldilay -lGL -lGLEW
PRE_TARGETDEPS         += ../lib/libdilay.a

SOURCES += \
           src/test-indexable.cpp \
           src/test-intersection.cpp \
           src/test-maybe.cpp \
           src/main.cpp \
           src/test-octree.cpp

HEADERS += \
           src/test-indexable.hpp \
           src/test-intersection.hpp \
           src/test-maybe.hpp \
           src/test-octree.hpp

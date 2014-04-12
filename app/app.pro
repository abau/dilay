! include( ../common.pri ) {
    error( Can not find .common.pri )
}

TEMPLATE               =  app
TARGET                 =  dilay
DESTDIR                =  ..
DEPENDPATH             += src 
INCLUDEPATH            += src ../lib/src
LIBS                   += -L../lib -ldilay -lGL -lGLEW
PRE_TARGETDEPS         += ../lib/libdilay.a

SOURCES                += src/main.cpp

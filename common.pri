VERSION                 = 1.0.1
CONFIG                 += debug_and_release warn_on object_parallel_to_source ordered c++14
QT                     += widgets opengl openglextensions xml
MOC_DIR                 = moc
OBJECTS_DIR             = obj
QMAKE_CXXFLAGS         += -DDILAY_VERSION=\\\"$$VERSION\\\" -DGLM_FORCE_RADIANS
QMAKE_CXXFLAGS_RELEASE += -DNDEBUG -Wno-unused-parameter -Wno-unused-variable
QMAKE_CXXFLAGS_DEBUG   += -Wall -Werror # -pg # -DDILAY_RENDER_OCTREE
QMAKE_LFLAGS_DEBUG     += # -pg

win32:INCLUDEPATH      += $$PWD/win32-data/glm/

unix {
  isEmpty (PREFIX) {
    PREFIX = /usr/local
  }
}

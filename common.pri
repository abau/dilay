CONFIG                 += release warn_on object_parallel_to_source ordered c++14
QT                     += widgets opengl openglextensions xml
MOC_DIR                 = moc
OBJECTS_DIR             = obj
QMAKE_CXXFLAGS         += -DGLM_FORCE_RADIANS
QMAKE_CXXFLAGS_RELEASE += -DNDEBUG -Wno-unused-parameter -Wno-unused-variable
QMAKE_CXXFLAGS_DEBUG   += -Wall -Werror # -pg # -DDILAY_RENDER_OCTREE
QMAKE_LFLAGS_DEBUG     += # -pg 

isEmpty (PREFIX) {
  PREFIX = /usr/local
}

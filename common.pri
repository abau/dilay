CONFIG                 += warn_on object_parallel_to_source ordered
QT                     += widgets opengl xml
MOC_DIR                =  moc
OBJECTS_DIR            =  obj
QMAKE_CXXFLAGS         += --std=c++14 -DGLM_FORCE_RADIANS 
QMAKE_CXXFLAGS_RELEASE += -DNDEBUG -Wno-unused-parameter -Wno-unused-variable
QMAKE_CXXFLAGS_DEBUG   += -Wall -Werror # -pg # -DDILAY_RENDER_OCTREE
QMAKE_LFLAGS_DEBUG     += # -pg 
TRANSLATIONS           =  i18n/dilay_de.ts

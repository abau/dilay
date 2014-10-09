CONFIG                 += warn_on debug object_parallel_to_source ordered
QT                     += widgets opengl xml
MOC_DIR                =  moc
OBJECTS_DIR            =  obj
QMAKE_CXXFLAGS         += --std=c++14 -Werror -DGLEW_STATIC -DGLM_FORCE_RADIANS 
QMAKE_CXXFLAGS_DEBUG   += # -pg # -DDILAY_RENDER_OCTREE
QMAKE_LFLAGS_DEBUG     += # -pg 
TRANSLATIONS           =  i18n/dilay_de.ts

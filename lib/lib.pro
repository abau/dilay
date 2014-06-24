! include( ../common.pri ) {
    error( Can not find .common.pri )
}

TEMPLATE               =  lib
TARGET                 =  dilay
DEPENDPATH             += src 
INCLUDEPATH            += src 
CONFIG                 += staticlib

SOURCES += \
           src/action/carve.cpp \
           src/action/delete-sphere-mesh.cpp \
           src/action/delete-winged-mesh.cpp \
           src/action/from-mesh.cpp \
           src/action/ids.cpp \
           src/action/modify-sphere-mesh.cpp \
           src/action/new-winged-mesh.cpp \
           src/action/new-sphere-mesh.cpp \
           src/action/on-post-processed-winged-mesh.cpp \
           src/action/subdivide.cpp \
           src/action/unit.cpp \
           src/adjacent-iterator.cpp \
           src/axis.cpp \
           src/camera.cpp \
           src/carve-brush.cpp \
           src/color.cpp \
           src/config.cpp \
           src/config-conversion.cpp \
           src/cursor.cpp \
           src/history.cpp \
           src/id.cpp \
           src/intersection.cpp \
           src/mesh.cpp \
           src/octree.cpp \
           src/opengl-util.cpp \
           src/partial-action/delete-edge-face.cpp \
           src/partial-action/delete-t-edges.cpp \
           src/partial-action/insert-edge-vertex.cpp \
           src/partial-action/modify-winged-edge.cpp \
           src/partial-action/modify-winged-face.cpp \
           src/partial-action/modify-winged-mesh.cpp \
           src/partial-action/modify-winged-vertex.cpp \
           src/partial-action/triangulate-6-gon.cpp \
           src/partial-action/triangulate-quad.cpp \
           src/primitive/aabox.cpp \
           src/primitive/plane.cpp \
           src/primitive/ray.cpp \
           src/primitive/sphere.cpp \
           src/primitive/triangle.cpp \
           src/renderer.cpp \
           src/render-mode.cpp \
           src/scene.cpp \
           src/selection.cpp \
           src/selection-mode.cpp \
           src/shader.cpp \
           src/sphere/node-intersection.cpp \
           src/sphere/mesh.cpp \
           src/state.cpp \
           src/subdivision-butterfly.cpp \
           src/time-delta.cpp \
           src/tool.cpp \
           src/tool/delete-mesh.cpp \
           src/tool/rotate.cpp \
           src/util.cpp \
           src/view/freeform-mesh-menu.cpp \
           src/view/gl-widget.cpp \
           src/view/light.cpp \
           src/view/main-widget.cpp \
           src/view/main-window.cpp \
           src/view/properties.cpp \
           src/view/properties/button.cpp \
           src/view/properties/selection.cpp \
           src/view/properties/widget.cpp \
           src/view/sphere-mesh-menu.cpp \
           src/view/tool-message.cpp \
           src/view/tool-options.cpp \
           src/view/util.cpp \
           src/view/vector-edit.cpp \
           src/winged/edge.cpp \
           src/winged/face.cpp \
           src/winged/face-intersection.cpp \
           src/winged/mesh.cpp \
           src/winged/util.cpp \
           src/winged/vertex.cpp \

HEADERS += \
           src/action.hpp \
           src/action/carve.hpp \
           src/action/delete-sphere-mesh.hpp \
           src/action/delete-winged-mesh.hpp \
           src/action/from-mesh.hpp \
           src/action/ids.hpp \
           src/action/modify-sphere-mesh.hpp \
           src/action/new-winged-mesh.hpp \
           src/action/new-sphere-mesh.hpp \
           src/action/on.hpp \
           src/action/on-post-processed-winged-mesh.hpp \
           src/action/subdivide.hpp \
           src/action/transformer.hpp \
           src/action/unit.hpp \
           src/action/unit/on.hpp \
           src/adjacent-iterator.hpp \
           src/axis.hpp \
           src/bitset.hpp \
           src/camera.hpp \
           src/carve-brush.hpp \
           src/color.hpp \
           src/config.hpp \
           src/config-conversion.hpp \
           src/cursor.hpp \
           src/dimension.hpp \
           src/fwd-winged.hpp \
           src/history.hpp \
           src/id.hpp \
           src/id-map.hpp \
           src/intersection.hpp \
           src/iterator.hpp \
           src/macro.hpp \
           src/mesh.hpp \
           src/mesh-type.hpp \
           src/octree.hpp \
           src/opengl-util.hpp \
           src/partial-action/delete-edge-face.hpp \
           src/partial-action/delete-t-edges.hpp \
           src/partial-action/insert-edge-vertex.hpp \
           src/partial-action/modify-winged-edge.hpp \
           src/partial-action/modify-winged-face.hpp \
           src/partial-action/modify-winged-mesh.hpp \
           src/partial-action/modify-winged-vertex.hpp \
           src/partial-action/triangulate-6-gon.hpp \
           src/partial-action/triangulate-quad.hpp \
           src/primitive/aabox.hpp \
           src/primitive/plane.hpp \
           src/primitive/ray.hpp \
           src/primitive/sphere.hpp \
           src/primitive/triangle.hpp \
           src/renderer.hpp \
           src/render-mode.hpp \
           src/scene.hpp \
           src/selection.hpp \
           src/selection-mode.hpp \
           src/shader.hpp \
           src/sphere/node-intersection.hpp \
           src/sphere/mesh.hpp \
           src/state.hpp \
           src/subdivision-butterfly.hpp \
           src/time-delta.hpp \
           src/tool.hpp \
           src/tool/delete-mesh.hpp \
           src/tool/rotate.hpp \
           src/util.hpp \
           src/variant.hpp \
           src/view/freeform-mesh-menu.hpp \
           src/view/gl-widget.hpp \
           src/view/light.hpp \
           src/view/main-widget.hpp \
           src/view/main-window.hpp \
           src/view/properties.hpp \
           src/view/properties/button.hpp \
           src/view/properties/selection.hpp \
           src/view/properties/widget.hpp \
           src/view/sphere-mesh-menu.hpp \
           src/view/tool-menu.hpp \
           src/view/tool-message.hpp \
           src/view/tool-options.hpp \
           src/view/util.hpp \
           src/view/vector-edit.hpp \
           src/winged/edge.hpp \
           src/winged/face.hpp \
           src/winged/face-intersection.hpp \
           src/winged/mesh.hpp \
           src/winged/util.hpp \
           src/winged/vertex.hpp \

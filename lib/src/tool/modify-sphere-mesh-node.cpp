#include <QDoubleSpinBox>
#include "tool/modify-sphere-mesh-node.hpp"
#include "mesh.hpp"
#include "primitive/sphere.hpp"
#include "view/vector-edit.hpp"
#include "tool/movement.hpp"
#include "config.hpp"
#include "intersection.hpp"
#include "state.hpp"

struct ToolModifySphereMeshNode::Impl {
  ToolModifySphereMeshNode*      self;
  ToolModifySphereMeshNode::Mode mode;
  Mesh                           mesh;
  PrimSphere                     meshGeometry;
  ToolMovement                   movement;
  QDoubleSpinBox*                radiusEdit;
  ViewVectorEdit*                positionEdit;

  Impl (ToolModifySphereMeshNode* s, Mode m) 
    : self     (s) 
    , mode     (m)
    , movement (s)
  {
  //  float initRadius = Config::get <float> ("/cache/tool/modify-sphere-mesh-node/radius", 0.5f);

  }

  ~Impl () {
  }

  /*
  void hover (const glm::ivec2& pos) {
    this->self->hover (IntersectionUtil::intersects ( State::camera ().ray (pos)
                                                    , this->meshGeometry, nullptr));
  }
  */

  static QString toolName (ToolModifySphereMeshNode::Mode mode) {
    switch (mode) {
      case ToolModifySphereMeshNode::Mode::NewMesh:
        return QObject::tr ("New Mesh");
      case ToolModifySphereMeshNode::Mode::NewNode:
        return QObject::tr ("New Node");
      case ToolModifySphereMeshNode::Mode::MoveNode:
        return QObject::tr ("Move Node");
    }
    assert (false);
  }
};

DELEGATE1_TOOL  (ToolModifySphereMeshNode, Mode)

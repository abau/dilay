#include <QDoubleSpinBox>
#include "tool/modify-sphere-mesh-node.hpp"
#include "mesh.hpp"
#include "primitive/sphere.hpp"
#include "view/vector-edit.hpp"
#include "tool/movement.hpp"
#include "view/tool-options.hpp"
#include "view/util.hpp"
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
  ConfigProxy                    config;

  Impl (ToolModifySphereMeshNode* s, Mode m) 
    : self     (s) 
    , mode     (m)
    , movement (s)
    , config   ("/cache/tool/modify-sphere-mesh-node/")
  {
    float initRadius = this->config.get <float> ("radius", 0.5f);

    // connect radius edit
    this->radiusEdit = this->self->toolOptions ()->add <QDoubleSpinBox>
                        ( QObject::tr ("Radius")
                        , ViewUtil::spinBox (0.001f, initRadius));
    ViewUtil::connect (this->radiusEdit, [this] (double r) { this->setRadius (float (r)); });

    // connect position edit
    this->positionEdit = this->self->toolOptions ()->add <ViewVectorEdit>
                          ( QObject::tr ("Position")
                          , new ViewVectorEdit (this->movement.position ()));
    QObject::connect (this->positionEdit, &ViewVectorEdit::vectorEdited, [this] 
        (const glm::vec3& p) { this->setPosition (p); });

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

  void setPosition (const glm::vec3& pos) {
    this->movement.position    (pos);
    this->meshGeometry.center  (pos);
    this->self->updateGlWidget ();
  }

  void setRadius (float radius) {
    this->meshGeometry.radius  (radius);
    this->self->updateGlWidget ();
    this->config.cache <float> ("radius", radius);
  }

  void setRadiusByMovement (const glm::ivec2& pos) {
    glm::vec3 radiusPoint;
    if (this->movement.onCameraPlane (pos, &radiusPoint)) {
      float d = glm::distance (radiusPoint, this->movement.position ());
      this->setRadius (d);
      this->radiusEdit->setValue (d);
    }
  }

};

DELEGATE1_TOOL  (ToolModifySphereMeshNode, Mode)

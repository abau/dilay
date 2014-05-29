#include <utility>
#include <glm/glm.hpp>
#include <QDoubleSpinBox>
#include <QMouseEvent>
#include "tool/util/radius.hpp"
#include "tool.hpp"
#include "tool/movement.hpp"
#include "view/tool-options.hpp"
#include "view/util.hpp"
#include "config.hpp"

struct ToolUtilRadius::Impl {
  ToolUtilRadius*     self;
  const ConfigProxy&  config;
  const ToolMovement& movement;
  float               _radius;
  QDoubleSpinBox&     radiusEdit;

  Impl (ToolUtilRadius* s, const ConfigProxy& c, const ToolMovement& m) 
    : self       (s) 
    , config     (c)
    , movement   (m)
    , _radius    (c.get <float> ("radius", 1.0f))
    , radiusEdit (ViewUtil::spinBox (0.001f, this->_radius))
  {
    s->toolOptions ().add <QDoubleSpinBox> ( QObject::tr ("Radius")
                                           , this->radiusEdit);
    ViewUtil::connect (this->radiusEdit, [this] (double r) { this->radius (float (r), false); });
  }

  ~Impl () {
    this->config.cache <float> ("radius", this->_radius);
  }

  float radius () const {
    return this->_radius;
  }

  void radius (float r, bool updateEdit = true) {
    this->_radius = r;
    if (updateEdit) {
      this->radiusEdit.setValue (r);
    }
  }

  bool runMouseMoveEvent (QMouseEvent& e) {
    if (e.modifiers ().testFlag (Qt::ControlModifier)) {
      glm::ivec2 pos = ViewUtil::toIVec2 (e);
      glm::vec3 radiusPoint;
      if (this->movement.onCameraPlane (pos, radiusPoint)) {
        float d = glm::distance (radiusPoint, this->movement.position ());
        this->radius            (d);
      }
    }
    return false;
  }
};

DELEGATE_BIG3_BASE ( ToolUtilRadius,(Tool& t, const ConfigProxy& p, const ToolMovement& m)
                   , (this,p,m),ToolUtil,(t))
DELEGATE_CONST (float, ToolUtilRadius, radius)
DELEGATE1      (void , ToolUtilRadius, radius, float)
DELEGATE1      (bool , ToolUtilRadius, runMouseMoveEvent, QMouseEvent&)

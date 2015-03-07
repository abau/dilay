#include <QDoubleSpinBox>
#include <QMouseEvent>
#include <glm/glm.hpp>
#include "camera.hpp"
#include "mesh.hpp"
#include "primitive/ray.hpp"
#include "render-mode.hpp"
#include "sculpt-brush/carve.hpp"
#include "state.hpp"
#include "tool/sculpt/behaviors.hpp"
#include "tool/util/movement.hpp"
#include "view/cursor.hpp"
#include "view/properties.hpp"
#include "view/tool/tip.hpp"
#include "view/util.hpp"
#include "winged/face-intersection.hpp"

struct ToolSculptDrag::Impl {
  ToolSculptDrag*  self;
  SculptBrushCarve brush;
  ViewCursor       cursor;
  ToolUtilMovement movement;

  std::vector <glm::vec3> draggedVertices;
  Mesh                    draggedVerticesMesh;

  Impl (ToolSculptDrag* s) 
    : self     (s)
    , movement ( this->self->state ().camera ()
               , glm::vec3 (0.0f)
               , MovementConstraint::Explicit )
  {
    this->draggedVerticesMesh.renderMode ().constantShading (true);
    this->draggedVerticesMesh.renderMode ().noDepthTest (true);
  }

  void runSetupBrush () {
    this->self->forceBrushSubdivision (true);
    this->brush.innerRadiusFactor (0.1);
    this->brush.subdivide         (true);
  }

  void runSetupCursor () {
    this->draggedVerticesMesh.color (this->cursor.color ());
  }

  void runSetupProperties (ViewPropertiesPart&) {}

  void runSetupToolTip (ViewToolTip& toolTip) {
    toolTip.add (ViewToolTip::MouseEvent::Left, QObject::tr ("Drag to sculpt"));
  }

  void updateDraggedVerticesMesh () {
    this->draggedVerticesMesh.reset ();

    if (this->draggedVertices.size () >= 2) {
      for (unsigned int i = 0; i < this->draggedVertices.size (); i++) {
        this->draggedVerticesMesh.addVertex (this->draggedVertices.at (i));

        if (i > 0) {
          this->draggedVerticesMesh.addIndex (i-1);
          this->draggedVerticesMesh.addIndex (i);
        }
      }
      this->draggedVerticesMesh.bufferData ();
    }
  }

  void runMousePressEvent (const QMouseEvent& e) {
    if (this->self->initializeDragMovement (this->movement, e)) {
      this->draggedVertices.clear        ();
      this->draggedVertices.emplace_back (this->movement.position ());
      this->draggedVerticesMesh.reset    ();
    }
  }

  void runMouseMoveEvent (const QMouseEvent& e) {
    const glm::ivec2& pos = ViewUtil::toIVec2 (e);

    if (e.buttons () == Qt::NoButton) {
      this->self->updateCursorByIntersection (e);
    }
    else if ( e.buttons () == Qt::LeftButton
           && this->draggedVertices.empty () == false
           && this->brush.hasPosition () 
           && this->movement.move     (pos)
           && this->brush.updatePosition (this->movement.position ()) )
    {
      this->draggedVertices.emplace_back (this->movement.position ());
      this->updateDraggedVerticesMesh    ();

    }
  }

  void runMouseReleaseEvent (const QMouseEvent& e) {
    if (e.button () == Qt::LeftButton) {
      auto initialSculpt = [this] {
        this->brush.useIntersection (false);
        this->brush.direction       (this->draggedVertices.at (1) - this->draggedVertices.at (0));
        this->brush.setPosition     (this->draggedVertices.at (0));
        this->self->sculpt ();
      };

      auto checkAgainstSegment = [this] (const glm::vec3& a, const glm::vec3& b) -> bool {
        const PrimRay ray         (a, b - a);
        const float   maxDistance (glm::length (b - a));

        WingedFaceIntersection intersection;
        if ( this->self->intersectsSelection (ray, intersection) 
          && intersection.distance () <= maxDistance ) 
        {
          this->brush.mesh            (&intersection.mesh     ());
          this->brush.face            (&intersection.face     ());
          this->brush.direction       (b - a);
          this->brush.setPosition     (intersection.position ());
          this->brush.useIntersection (true);
          this->self->sculpt ();
          return true;
        }
        else {
          return false;
        }
      };

      auto checkAgainstDraggedVertices = [this,&checkAgainstSegment] () -> bool {
        for (unsigned int i = 1; i < this->draggedVertices.size (); i++) {
          if (checkAgainstSegment ( this->draggedVertices.at (i-1)
                                  , this->draggedVertices.at (i) )) 
          {
            return true;
          }
        }
        return false;
      };

      if (this->draggedVertices.empty () == false) {
        this->brush.intensityFactor (1.0f / this->brush.radius ());

        initialSculpt ();

        while (checkAgainstDraggedVertices ()) {}
      }
      this->draggedVertices.clear     ();
      this->draggedVerticesMesh.reset ();
    }
  }

  void runRender () const {
    if (this->draggedVertices.size () >= 2) {
      this->draggedVerticesMesh.renderLines (this->self->state ().camera ());
    }
  }
};

DELEGATE_TOOL_SCULPT_BEHAVIOR                         (ToolSculptDrag)
DELEGATE_TOOL_SCULPT_BEHAVIOR_RUN_MOUSE_RELEASE_EVENT (ToolSculptDrag)
DELEGATE_TOOL_SCULPT_BEHAVIOR_RUN_RENDER              (ToolSculptDrag)

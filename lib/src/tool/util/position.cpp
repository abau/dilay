#include <utility>
#include <glm/glm.hpp>
#include "tool/util/position.hpp"
#include "tool.hpp"
#include "tool/movement.hpp"
#include "view/vector-edit.hpp"
#include "view/tool-options.hpp"

struct ToolUtilPosition::Impl {
  ToolUtilPosition*  self;
  ToolMovement       movement;
  ViewVectorEdit&    vectorEdit;

  Impl (ToolUtilPosition* s) 
    : self       (s) 
    , movement   (s->tool ())
    , vectorEdit (*new ViewVectorEdit (this->movement.position ()))
  {
    s->toolOptions ().add <ViewVectorEdit> ( QObject::tr ("Position")
                                           , this->vectorEdit);

    QObject::connect (&this->vectorEdit, &ViewVectorEdit::vectorEdited, [this] 
      (const glm::vec3& p) { this->movement.position (p); });
  }

  const glm::vec3& position () const {
    return this->movement.position ();
  }

  void position (const glm::vec3& pos) {
    this->movement.position (pos);
    this->vectorEdit.vector (pos);
  }

  bool runMouseMoveEvent (QMouseEvent& e) {
    if (this->self->tool ().isDraged ()) {
      if (this->movement.byMouseEvent (e)) {
        this->position (this->movement.position ());
        return true;
      }
    }
    return false;
  }
};

DELEGATE_BIG3_BASE ( ToolUtilPosition,(Tool& t), (this),ToolUtil,(t))
GETTER_CONST   (const ToolMovement&, ToolUtilPosition, movement)
DELEGATE_CONST (const glm::vec3&   , ToolUtilPosition, position)
DELEGATE1      (void               , ToolUtilPosition, position, const glm::vec3&)
DELEGATE1      (bool               , ToolUtilPosition, runMouseMoveEvent, QMouseEvent&)

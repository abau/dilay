#include "action/unit.hpp"
#include "action/unit/template.hpp"

struct ActionUnit :: Impl {
  ActionUnitTemplate <Action> unitTemplate;

  void undo () {
    this->unitTemplate.forallReverse ([] (Action& a) { a.undo (); });
  }

  void redo () {
    this->unitTemplate.forall ([] (Action& a) { a.redo (); });
  }
};

DELEGATE_ACTION_BIG6      (ActionUnit)
DELEGATE_TO_UNIT_TEMPLATE (ActionUnit,Action)

DELEGATE  (void, ActionUnit, undo)
DELEGATE  (void, ActionUnit, redo)

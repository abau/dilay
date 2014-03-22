#include "view/properties.hpp"
#include "view/property/selection.hpp"
#include "view/property/camera.hpp"
#include "view/property/movement.hpp"

struct ViewProperties::Impl {
  ViewProperties* self;

  Impl (ViewProperties* s) : self (s) {
    s->setFocusPolicy (Qt::NoFocus);

    s->addItem (new ViewPropertySelection, QObject::tr ("Selection"));
    s->addItem (new ViewPropertyCamera   , QObject::tr ("Camera"));
    s->addItem (new ViewPropertyMovement , QObject::tr ("Movement"));

  }
};

DELEGATE_BIG3_SELF (ViewProperties)

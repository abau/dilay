#include <QRadioButton>
#include "view/properties/selection.hpp"
#include "view/util.hpp"
#include "selection-mode.hpp"

struct ViewPropertiesSelection::Impl {
  ViewPropertiesSelection* self;
  QRadioButton&            freeformMeshButton;
  QRadioButton&            sphereMeshButton;
  QRadioButton&            sphereMeshNodeButton;

  Impl (ViewPropertiesSelection* s) 
    : self                 (s) 
    , freeformMeshButton   (ViewUtil::radioButton (tr ("Freeform Mesh"), true, true))
    , sphereMeshButton     (ViewUtil::radioButton (tr ("Sphere Mesh"), true))
    , sphereMeshNodeButton (ViewUtil::radioButton (tr ("Sphere Mesh Node"), true))
  {
    this->self->setLabel  (tr ("Selection"));
    this->self->addWidget (this->freeformMeshButton);
    this->self->addWidget (this->sphereMeshButton);
    this->self->addWidget (this->sphereMeshNodeButton);

    QObject::connect (&this->freeformMeshButton, &QRadioButton::toggled, [this] (bool checked) {
      if (checked) {
        emit this->self->selectionModeChanged (SelectionMode::Freeform);
      }
    });
    QObject::connect (&this->sphereMeshButton, &QRadioButton::toggled, [this] (bool checked) {
      if (checked) {
        emit this->self->selectionModeChanged (SelectionMode::Sphere);
      }
    });
    QObject::connect (&this->sphereMeshNodeButton, &QRadioButton::toggled, [this] (bool checked) {
      if (checked) {
        emit this->self->selectionModeChanged (SelectionMode::SphereNode);
      }
    });
  }

  SelectionMode selectionMode () const {
    if (this->freeformMeshButton.isChecked ()) {
      return SelectionMode::Freeform;
    }
    else if (this->sphereMeshButton.isChecked ()) {
      return SelectionMode::Sphere;
    }
    else if (this->sphereMeshNodeButton.isChecked ()) {
      return SelectionMode::SphereNode;
    }
    assert (false);
  }
};

DELEGATE_BIG3_SELF (ViewPropertiesSelection)
DELEGATE_CONST (SelectionMode, ViewPropertiesSelection, selectionMode)

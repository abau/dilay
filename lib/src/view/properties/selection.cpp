#include <QRadioButton>
#include "selection-mode.hpp"
#include "view/properties/selection.hpp"
#include "view/util.hpp"

struct ViewPropertiesSelection::Impl {
  ViewPropertiesSelection* self;
  QRadioButton&            freeformMeshButton;

  Impl (ViewPropertiesSelection* s) 
    : self                 (s) 
    , freeformMeshButton   (ViewUtil::radioButton (tr ("Freeform Mesh"), true))
  {
    this->self->setLabel  (tr ("Selection"));
    this->self->addWidget (this->freeformMeshButton);

    QObject::connect (&this->freeformMeshButton, &QRadioButton::toggled, [this] (bool checked) {
      if (checked) {
        emit this->self->selectionModeChanged (SelectionMode::Freeform);
      }
    });
  }

  SelectionMode selectionMode () const {
    if (this->freeformMeshButton.isChecked ()) {
      return SelectionMode::Freeform;
    }
    std::abort ();
  }
};

DELEGATE_BIG3_SELF (ViewPropertiesSelection)
DELEGATE_CONST (SelectionMode, ViewPropertiesSelection, selectionMode)

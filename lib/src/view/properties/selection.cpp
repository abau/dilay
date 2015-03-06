#include <QRadioButton>
#include "selection-mode.hpp"
#include "view/properties/selection.hpp"
#include "view/util.hpp"

struct ViewPropertiesSelection::Impl {
  ViewPropertiesSelection* self;
  QRadioButton&            freeformMeshButton;

  Impl (ViewPropertiesSelection* s) 
    : self               (s) 
    , freeformMeshButton (ViewUtil::radioButton (tr ("Freeform mesh"), true))
  {
    this->self->label       (tr ("Selection"));
    this->self->body ().add (this->freeformMeshButton);

    QObject::connect (&this->freeformMeshButton, &QRadioButton::toggled, [this] (bool checked) {
      if (checked) {
        emit this->self->selectionModeChanged (SelectionMode::WingedMesh);
      }
    });
  }

  SelectionMode selectionMode () const {
    if (this->freeformMeshButton.isChecked ()) {
      return SelectionMode::WingedMesh;
    }
    std::abort ();
  }
};

DELEGATE_BIG3_SELF (ViewPropertiesSelection)
DELEGATE_CONST (SelectionMode, ViewPropertiesSelection, selectionMode)

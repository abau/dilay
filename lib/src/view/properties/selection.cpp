#include <QRadioButton>
#include "selection-mode.hpp"
#include "view/properties/selection.hpp"
#include "view/util.hpp"

struct ViewPropertiesSelection::Impl {
  ViewPropertiesSelection* self;
  QRadioButton&            polyMeshButton;

  Impl (ViewPropertiesSelection* s) 
    : self             (s) 
    , polyMeshButton   (ViewUtil::radioButton (tr ("PolyMesh"), true))
  {
    this->self->label     (tr ("Selection"));
    this->self->addWidget (this->polyMeshButton);

    QObject::connect (&this->polyMeshButton, &QRadioButton::toggled, [this] (bool checked) {
      if (checked) {
        emit this->self->selectionModeChanged (SelectionMode::WingedMesh);
      }
    });
  }

  SelectionMode selectionMode () const {
    if (this->polyMeshButton.isChecked ()) {
      return SelectionMode::WingedMesh;
    }
    std::abort ();
  }
};

DELEGATE_BIG3_SELF (ViewPropertiesSelection)
DELEGATE_CONST (SelectionMode, ViewPropertiesSelection, selectionMode)

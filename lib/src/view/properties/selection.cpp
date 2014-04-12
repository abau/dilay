#include <QRadioButton>
#include <QCheckBox>
#include "view/properties/selection.hpp"
#include "view/util.hpp"
#include "mesh-type.hpp"

struct ViewPropertiesSelection::Impl {
  ViewPropertiesSelection* self;
  QRadioButton*            freeformMeshButton;
  QRadioButton*            sphereMeshButton;
  QCheckBox*               hideOthersBox;

  Impl (ViewPropertiesSelection* s) : self (s) {
    this->self->setLabel (tr ("Selection"));

    this->freeformMeshButton = ViewUtil::radioButton (tr ("Freeform Mesh"), true, true);
    this->sphereMeshButton   = ViewUtil::radioButton (tr ("Sphere Mesh"), true);
    this->hideOthersBox      = ViewUtil::checkBox    (tr ("Hide Others"), true, false);
    
    this->self->addWidget (this->freeformMeshButton);
    this->self->addWidget (this->sphereMeshButton);
    this->self->addWidget (this->hideOthersBox);

    QObject::connect (this->freeformMeshButton, &QRadioButton::toggled, [this] (bool checked) {
      if (checked) {
        emit this->self->selectionChanged (MeshType::Freeform);
      }
    });
    QObject::connect (this->sphereMeshButton, &QRadioButton::toggled, [this] (bool checked) {
      if (checked) {
        emit this->self->selectionChanged (MeshType::Sphere);
      }
    });
    QObject::connect (this->hideOthersBox, &QCheckBox::toggled, [this] (bool checked) {
      emit this->self->hideOthersChanged (checked);
    });
  }

  bool selected (MeshType t) const {
    if (  (t == MeshType::Freeform && this->freeformMeshButton->isChecked ())
       || (t == MeshType::Sphere   && this->sphereMeshButton  ->isChecked ())) {
      return true;
    }
    return false;
  }

  bool show (MeshType t) const {
    return this->selected (t) || (! this->hideOthersBox->isChecked ());
  }
};

DELEGATE_BIG3_SELF (ViewPropertiesSelection)
DELEGATE1_CONST (bool, ViewPropertiesSelection, selected, MeshType)
DELEGATE1_CONST (bool, ViewPropertiesSelection, show, MeshType)

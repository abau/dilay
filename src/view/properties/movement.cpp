#include <QRadioButton>
#include "view/properties/movement.hpp"
#include "view/util.hpp"

struct ViewPropertiesMovement::Impl {
  ViewPropertiesMovement* self;
  QRadioButton*           xyButton;
  QRadioButton*           yzButton;
  QRadioButton*           xzButton;
  QRadioButton*           cameraButton;

  Impl (ViewPropertiesMovement* s) : self (s) {
    this->self->setLabel (tr ("Movement"));

    this->xyButton     = ViewUtil::radioButton (tr ("XY Plane"), true);
    this->yzButton     = ViewUtil::radioButton (tr ("YZ Plane"), true);
    this->xzButton     = ViewUtil::radioButton (tr ("XZ Plane"), true);
    this->cameraButton = ViewUtil::radioButton (tr ("Camera Plane"), true, true);

    this->self->addWidget (this->xyButton);
    this->self->addWidget (this->yzButton);
    this->self->addWidget (this->xzButton);
    this->self->addWidget (this->cameraButton);
  }

  bool xy     () const { return this->xyButton->isChecked (); }
  bool yz     () const { return this->yzButton->isChecked (); }
  bool xz     () const { return this->xzButton->isChecked (); }
  bool camera () const { return this->cameraButton->isChecked (); }
  bool x      () const { return this->yz (); }
  bool y      () const { return this->xz (); }
  bool z      () const { return this->xy (); }
};

DELEGATE_BIG3_SELF (ViewPropertiesMovement)
DELEGATE_CONST (bool, ViewPropertiesMovement, xy)
DELEGATE_CONST (bool, ViewPropertiesMovement, yz)
DELEGATE_CONST (bool, ViewPropertiesMovement, xz)
DELEGATE_CONST (bool, ViewPropertiesMovement, camera)
DELEGATE_CONST (bool, ViewPropertiesMovement, x)
DELEGATE_CONST (bool, ViewPropertiesMovement, y)
DELEGATE_CONST (bool, ViewPropertiesMovement, z)

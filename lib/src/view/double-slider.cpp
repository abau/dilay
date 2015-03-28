#include <glm/glm.hpp>
#include "view/double-slider.hpp"

struct ViewDoubleSlider::Impl {
  ViewDoubleSlider*  self;
  const unsigned int factor;

  Impl (ViewDoubleSlider* s, unsigned short numDecimals) 
    : self   (s)
    , factor (glm::pow (10, numDecimals))
  {
    QObject::connect (this->self, &QSlider::valueChanged, [this] (int i) {
      emit this->self->doubleValueChanged (double (i) / this->factor);
    });
  }

  double doubleValue () const {
    return double (this->self->value ()) / this->factor;
  }

  double doubleSingleStep () const {
    return double (this->self->singleStep ()) / this->factor;
  }

  void setValue (double v) {
    this->self->QSlider::setValue (int (v * this->factor));
  }

  void setRange (double min, double max) {
    this->self->QSlider::setRange (int (min * this->factor), int (max * this->factor));
  }

  void setSingleStep (double v) {
    this->self->QSlider::setSingleStep (int (v * this->factor));
  }

  void setPageStep (double v) {
    this->self->QSlider::setPageStep (int (v * this->factor));
  }
};

DELEGATE1_BIG2_SELF (ViewDoubleSlider, unsigned short)
DELEGATE_CONST (double, ViewDoubleSlider, doubleValue)
DELEGATE_CONST (double, ViewDoubleSlider, doubleSingleStep)
DELEGATE1      (void  , ViewDoubleSlider, setValue, double)
DELEGATE2      (void  , ViewDoubleSlider, setRange, double, double)
DELEGATE1      (void  , ViewDoubleSlider, setSingleStep, double)
DELEGATE1      (void  , ViewDoubleSlider, setPageStep, double)

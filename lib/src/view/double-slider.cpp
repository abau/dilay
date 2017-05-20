/* This file is part of Dilay
 * Copyright © 2015-2017 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <glm/glm.hpp>
#include "../util.hpp"
#include "view/double-slider.hpp"

struct ViewDoubleSlider::Impl
{
  ViewDoubleSlider*    self;
  const unsigned int   factor;
  const unsigned short order;

  Impl (ViewDoubleSlider* s, unsigned short numDecimals, unsigned short o)
    : self (s)
    , factor (glm::pow (10, numDecimals))
    , order (o)
  {
    assert (this->order > 0);

    QObject::connect (this->self, &QSlider::valueChanged, [this](int i) {
      emit this->self->doubleValueChanged (this->toDouble (i, false));
    });
  }

  double toDouble (int value, bool forceLinear) const
  {
    const unsigned short o = forceLinear ? 1 : this->order;

    if (o == 1)
    {
      return double(value) / this->factor;
    }
    else
    {
      const double min = double(this->self->minimum ()) / this->factor;
      const double max = double(this->self->maximum ()) / this->factor;
      const double d = double(value) / this->factor;
      const double norm = (d - min) / (max - min);
      const double result = min + (glm::pow (norm, float(o)) * (max - min));

      return result;
    }
  }

  int toInt (double value, bool forceLinear) const
  {
    const unsigned short o = forceLinear ? 1 : this->order;

    if (o == 1)
    {
      return int(std::round (value * this->factor));
    }
    else
    {
      const double min = double(this->self->minimum ()) / this->factor;
      const double max = double(this->self->maximum ()) / this->factor;
      const double norm = (value - min) / (max - min);
      const double slope = norm <= Util::epsilon () ? 0.0f : glm::pow (norm, 1.0f / float(o));

      return int(std::round (this->factor * (min + (slope * (max - min)))));
    }
  }

  double doubleValue () const { return this->toDouble (this->self->value (), false); }

  double doubleSingleStep () const { return this->toDouble (this->self->singleStep (), true); }

  void setDoubleValue (double v) { this->self->setValue (this->toInt (v, false)); }

  void setDoubleRange (double min, double max)
  {
    this->self->setRange (this->toInt (min, true), this->toInt (max, true));
  }

  void setDoubleSingleStep (double v) { this->self->setSingleStep (this->toInt (v, true)); }

  void setDoublePageStep (double v) { this->self->setPageStep (this->toInt (v, true)); }

  int intValue () const { return this->self->QSlider::value (); }

  int intSingleStep () const { return this->self->QSlider::singleStep (); }

  void setIntValue (int v) { this->self->QSlider::setValue (v); }

  void setIntRange (int min, int max) { this->self->QSlider::setRange (min, max); }

  void setIntSingleStep (int v) { this->self->QSlider::setSingleStep (v); }

  void setIntPageStep (int v) { this->self->QSlider::setPageStep (v); }

  int value () const { return this->intValue (); }

  int singleStep () const { return this->intSingleStep (); }

  void setValue (int v) { this->setIntValue (v); }

  void setRange (int min, int max) { this->setIntRange (min, max); }

  void setSingleStep (int v) { this->setIntSingleStep (v); }

  void setPageStep (int v) { this->setIntPageStep (v); }
};

DELEGATE2_BIG2_SELF (ViewDoubleSlider, unsigned short, unsigned short)
DELEGATE_CONST (double, ViewDoubleSlider, doubleValue)
DELEGATE_CONST (double, ViewDoubleSlider, doubleSingleStep)
DELEGATE1 (void, ViewDoubleSlider, setDoubleValue, double)
DELEGATE2 (void, ViewDoubleSlider, setDoubleRange, double, double)
DELEGATE1 (void, ViewDoubleSlider, setDoubleSingleStep, double)
DELEGATE1 (void, ViewDoubleSlider, setDoublePageStep, double)
DELEGATE_CONST (int, ViewDoubleSlider, intValue)
DELEGATE_CONST (int, ViewDoubleSlider, intSingleStep)
DELEGATE1 (void, ViewDoubleSlider, setIntValue, int)
DELEGATE2 (void, ViewDoubleSlider, setIntRange, int, int)
DELEGATE1 (void, ViewDoubleSlider, setIntSingleStep, int)
DELEGATE1 (void, ViewDoubleSlider, setIntPageStep, int)
DELEGATE_CONST (int, ViewDoubleSlider, value)
DELEGATE_CONST (int, ViewDoubleSlider, singleStep)
DELEGATE1 (void, ViewDoubleSlider, setValue, int)
DELEGATE2 (void, ViewDoubleSlider, setRange, int, int)
DELEGATE1 (void, ViewDoubleSlider, setSingleStep, int)
DELEGATE1 (void, ViewDoubleSlider, setPageStep, int)

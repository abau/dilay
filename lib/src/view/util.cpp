#include <QButtonGroup>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QFrame>
#include <QLineEdit>
#include <QMouseEvent>
#include <QPushButton>
#include <QRadioButton>
#include <QSpinBox>
#include <QToolButton>
#include <glm/glm.hpp>
#include <limits>
#include "view/double-slider.hpp"
#include "view/util.hpp"

QSpinBox& ViewUtil :: spinBox (int min, int value, int max, int stepSize) {
  QSpinBox& spinBox = *new QSpinBox;
  spinBox.setRange      (min, max);
  spinBox.setValue      (value);
  spinBox.setSingleStep (stepSize);
  return spinBox;
}

QDoubleSpinBox& ViewUtil :: spinBox (float min, float value, float max, float stepSize) {
  QDoubleSpinBox& spinBox = *new QDoubleSpinBox;
  spinBox.setValue      (double (value));
  spinBox.setRange      (min, max);
  spinBox.setSingleStep (stepSize);
  spinBox.setDecimals   (2);
  return spinBox;
}

QPushButton& ViewUtil :: pushButton (const QString& label, bool isDefaultButton) {
  QPushButton& button = *new QPushButton (label);
  button.setDefault (isDefaultButton);
  return button;
}

QToolButton& ViewUtil :: toolButton (const QString& label) {
  QToolButton& button = *new QToolButton ();
  button.setText (label);
  return button;
}

QRadioButton& ViewUtil :: radioButton (const QString& label, bool isChecked) {
  QRadioButton& button = *new QRadioButton (label);
  button.setChecked (isChecked);
  return button;
}

QCheckBox& ViewUtil :: checkBox (const QString& label, bool isChecked) {
  QCheckBox& box = *new QCheckBox (label);
  box.setChecked (isChecked);
  return box;
}

ViewDoubleSlider& ViewUtil :: slider ( float min, float value, float max
                                     , float stepSize, unsigned short order )
{
  ViewDoubleSlider& slider = *new ViewDoubleSlider (2, order);
  slider.setDoubleRange       (min, max);
  slider.setDoubleValue       (value);
  slider.setDoubleSingleStep  (stepSize);
  slider.setDoublePageStep    (max);
  slider.setTracking          (true);
  slider.setOrientation       (Qt::Horizontal);
  return slider;
}

QFrame& ViewUtil :: horizontalLine () {
  QFrame& frame = *new QFrame;
  frame.setFrameShape  (QFrame::HLine);
  frame.setFrameShadow (QFrame::Sunken);
  return frame;
}

glm::uvec2 ViewUtil :: toUVec2 (const QPoint& p) {
  assert (p.x () >= 0);
  assert (p.y () >= 0);
  return glm::uvec2 (p.x (), p.y ());
}

glm::uvec2 ViewUtil :: toUVec2 (const QMouseEvent& e) {
  return ViewUtil::toUVec2 (e.pos ());
}

glm::ivec2 ViewUtil :: toIVec2 (const QPoint& p) {
  return glm::ivec2 (p.x (), p.y ());
}

glm::ivec2 ViewUtil :: toIVec2 (const QMouseEvent& e) {
  return ViewUtil::toIVec2 (e.pos ());
}

void ViewUtil :: connect (const QSpinBox& s, const std::function <void (int)>& f) {
  void (QSpinBox::* ptr)(int) = &QSpinBox::valueChanged;
  QObject::connect (&s, ptr, f);
}

void ViewUtil :: connect (const QDoubleSpinBox& s, const std::function <void (double)>& f) {
  void (QDoubleSpinBox::* ptr)(double) = &QDoubleSpinBox::valueChanged;
  QObject::connect (&s, ptr, f);
}

void ViewUtil :: connect (const QPushButton& b, const std::function <void ()>& f) {
  QObject::connect (&b, &QPushButton::released, f);
}

void ViewUtil :: connect (const QButtonGroup& g, const std::function <void (int)>& f) {
  void (QButtonGroup::* ptr)(int) = &QButtonGroup::buttonReleased;
  QObject::connect (&g, ptr, f);
}

void ViewUtil :: connect (const QCheckBox& c, const std::function <void (bool)>& f) {
  QObject::connect (&c, &QCheckBox::stateChanged, [f] (int state) {
    if (state == Qt::Unchecked) {
      f (false);
    }
    else if (state == Qt::Checked) {
      f (true);
    }
  });
}

void ViewUtil :: connect (const QRadioButton& r, const std::function <void (bool)>& f) {
  QObject::connect (&r, &QRadioButton::clicked, f);
}

void ViewUtil :: connect (const ViewDoubleSlider& s, const std::function <void (float)>& f) {
  QObject::connect (&s, &ViewDoubleSlider::doubleValueChanged, f);
}

QWidget& ViewUtil :: stretcher (bool horizontal, bool vertical) {
  assert (horizontal || vertical);
  QWidget& widget = *new QWidget ();
  widget.setSizePolicy ( horizontal ? QSizePolicy::Expanding : QSizePolicy::Preferred
                       , vertical   ? QSizePolicy::Expanding : QSizePolicy::Preferred );
  return widget;
}

void ViewUtil :: deselect (QAbstractSpinBox& spinBox) {
  spinBox.findChild <QLineEdit*> ()->deselect ();
}

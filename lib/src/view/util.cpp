#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QMouseEvent>
#include <QPushButton>
#include <QRadioButton>
#include <QSpinBox>
#include <QToolButton>
#include <glm/glm.hpp>
#include <limits>
#include "view/util.hpp"

QSpinBox& ViewUtil :: spinBox (int min, int value, int max) {
  QSpinBox& spinBox = *new QSpinBox;
  spinBox.setRange (min, max);
  spinBox.setValue (value);
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

QWidget& ViewUtil :: stretcher (bool horizontal, bool vertical) {
  assert (horizontal || vertical);
  QWidget& widget = *new QWidget ();
  widget.setSizePolicy ( horizontal ? QSizePolicy::Expanding : QSizePolicy::Preferred
                       , vertical   ? QSizePolicy::Expanding : QSizePolicy::Preferred );
  return widget;
}

#include <limits>
#include <glm/glm.hpp>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QToolButton>
#include <QRadioButton>
#include <QCheckBox>
#include <QMouseEvent>
#include "view/util.hpp"

QSpinBox& ViewUtil :: spinBox (int min, int value, int max) {
  QSpinBox& spinBox = *new QSpinBox;
  spinBox.setRange (min, max);
  spinBox.setValue (value);
  return spinBox;
}

QDoubleSpinBox& ViewUtil :: spinBox (float min, float value, float max) {
  QDoubleSpinBox& spinBox = ViewUtil::spinBox (min,value);
  spinBox.setMaximum (double (max));
  return spinBox;
}

QDoubleSpinBox& ViewUtil :: spinBox (float min, float value) {
  QDoubleSpinBox& spinBox = ViewUtil::spinBox (value);
  spinBox.setMinimum (double (min));
  return spinBox;
}

QDoubleSpinBox& ViewUtil :: spinBox (float value) {
  QDoubleSpinBox& spinBox = *new QDoubleSpinBox;
  spinBox.setValue      (double (value));
  spinBox.setRange      (-1000.0, 1000.0);
  spinBox.setSingleStep (0.1);
  spinBox.setDecimals   (3);
  return spinBox;
}

QPushButton& ViewUtil :: pushButton (const QString& label, bool ignoreFocus) {
  QPushButton& button = *new QPushButton (label);
  if (ignoreFocus) { 
    button.setFocusPolicy (Qt::NoFocus);
  }
  return button;
}

QToolButton& ViewUtil :: toolButton (const QString& label, bool ignoreFocus) {
  QToolButton& button = *new QToolButton ();
  button.setText (label);
  if (ignoreFocus) { 
    button.setFocusPolicy (Qt::NoFocus);
  }
  return button;
}

QRadioButton& ViewUtil :: radioButton (const QString& label, bool ignoreFocus, bool isChecked) {
  QRadioButton& button = *new QRadioButton (label);
  button.setChecked (isChecked);
  if (ignoreFocus) { 
    button.setFocusPolicy (Qt::NoFocus);
  }
  return button;
}

QCheckBox& ViewUtil :: checkBox (const QString& label, bool ignoreFocus, bool isChecked) {
  QCheckBox& box = *new QCheckBox (label);
  box.setChecked (isChecked);
  if (ignoreFocus) { 
    box.setFocusPolicy (Qt::NoFocus);
  }
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

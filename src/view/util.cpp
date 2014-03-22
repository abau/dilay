#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QPushButton>
#include "view/util.hpp"

QSpinBox* ViewUtil :: spinBox (int min, int value, int max) {
  QSpinBox* spinBox = new QSpinBox;
  spinBox->setRange (min, max);
  spinBox->setValue (value);
  return spinBox;
}

QDoubleSpinBox* ViewUtil :: spinBox (float min, float value, float step, float max) {
  QDoubleSpinBox* spinBox = new QDoubleSpinBox;
  spinBox->setRange       (double (min), double (max));
  spinBox->setValue       (double (value));
  spinBox->setSingleStep  (double (step));
  return spinBox;
}

QPushButton* ViewUtil :: pushButton (const QString& label) {
  QPushButton* button = new QPushButton (label);
  return button;
}

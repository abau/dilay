#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QToolButton>
#include <QRadioButton>
#include <QCheckBox>
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

QPushButton* ViewUtil :: pushButton (const QString& label, bool ignoreFocus) {
  QPushButton* button = new QPushButton (label);
  if (ignoreFocus) { 
    button->setFocusPolicy (Qt::NoFocus);
  }
  return button;
}

QToolButton* ViewUtil :: toolButton (const QString& label, bool ignoreFocus) {
  QToolButton* button = new QToolButton ();
  button->setText (label);
  if (ignoreFocus) { 
    button->setFocusPolicy (Qt::NoFocus);
  }
  return button;
}

QRadioButton* ViewUtil :: radioButton (const QString& label, bool ignoreFocus, bool isChecked) {
  QRadioButton* button = new QRadioButton (label);
  button->setChecked (isChecked);
  if (ignoreFocus) { 
    button->setFocusPolicy (Qt::NoFocus);
  }
  return button;
}

QCheckBox* ViewUtil :: checkBox (const QString& label, bool ignoreFocus, bool isChecked) {
  QCheckBox* box = new QCheckBox (label);
  box->setChecked (isChecked);
  if (ignoreFocus) { 
    box->setFocusPolicy (Qt::NoFocus);
  }
  return box;
}

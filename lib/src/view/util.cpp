/* This file is part of Dilay
 * Copyright © 2015,2016 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <QButtonGroup>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QDoubleValidator>
#include <QIntValidator>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QRadioButton>
#include <QSlider>
#include <QToolButton>
#include <glm/glm.hpp>
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

QSlider& ViewUtil :: slider (int min, int value, int max) {
  QSlider& slider = *new QSlider;
  slider.setRange       (min, max);
  slider.setValue       (value);
  slider.setSingleStep  (1);
  slider.setPageStep    (1);
  slider.setTracking    (true);
  slider.setOrientation (Qt::Horizontal);
  return slider;
}

ViewDoubleSlider& ViewUtil :: slider ( unsigned short numDecimals
                                     , float min, float value, float max
                                     , unsigned short order )
{
  ViewDoubleSlider& slider = *new ViewDoubleSlider (numDecimals, order);
  slider.setDoubleRange       (min, max);
  slider.setDoubleValue       (value);
  slider.setDoubleSingleStep  ((max - min) / 10.0f);
  slider.setDoublePageStep    ((max - min) / 10.0f);
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

QWidget& ViewUtil :: emptyWidget () {
  return *new QWidget;
}

QLineEdit& ViewUtil :: lineEdit (float value, unsigned short numDecimals) {
  return ViewUtil::lineEdit ( std::numeric_limits <float>::lowest (), value
                            , std::numeric_limits <float>::max (), numDecimals );
}

QLineEdit& ViewUtil :: lineEdit (float min, float value, float max, unsigned short numDecimals) {
  QLineEdit&        edit      = *new QLineEdit;
  QDoubleValidator& validator = *new QDoubleValidator (&edit);

  validator.setRange (min, max, numDecimals);
  edit.setValidator (&validator);
  edit.setText (QString::number (value, 'f', numDecimals));

  return edit;
}

QLineEdit& ViewUtil :: lineEdit (int value) {
  QLineEdit& edit = *new QLineEdit;

  edit.setText (QString::number (value));

  return edit;
}

QLineEdit& ViewUtil :: lineEdit (int min, int value, int max) {
  QLineEdit&     edit      = ViewUtil::lineEdit (value);
  QIntValidator& validator = *new QIntValidator (&edit);

  validator.setRange (min, max);
  edit.setValidator (&validator);

  return edit;
}

glm::uvec2 ViewUtil :: toUVec2 (const QPoint& p) {
  assert (p.x () >= 0);
  assert (p.y () >= 0);
  return glm::uvec2 (p.x (), p.y ());
}

glm::ivec2 ViewUtil :: toIVec2 (const QPoint& p) {
  return glm::ivec2 (p.x (), p.y ());
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

void ViewUtil :: connect (const QSlider& s, const std::function <void (int)>& f) {
  QObject::connect (&s, &QSlider::valueChanged, f);
}

void ViewUtil :: connect (const ViewDoubleSlider& s, const std::function <void (float)>& f) {
  QObject::connect (&s, &ViewDoubleSlider::doubleValueChanged, f);
}

void ViewUtil :: connectFloat (const QLineEdit& e, const std::function <void (float)>& f) {
  QObject::connect (&e, &QLineEdit::textEdited, [&e,f] (const QString& text) {
    if (e.hasAcceptableInput ()) {
      bool  ok;
      float value = text.toFloat (&ok);
      assert (ok);
      f (value);
    }
  });
}

void ViewUtil :: connectInt (const QLineEdit& e, const std::function <void (int)>& f) {
  QObject::connect (&e, &QLineEdit::textEdited, [&e,f] (const QString& text) {
    if (e.hasAcceptableInput ()) {
      bool  ok;
      float value = text.toInt (&ok);
      assert (ok);
      f (value);
    }
  });
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

bool ViewUtil :: question (QWidget& parent, const QString& label) {
  return QMessageBox::question (&parent, label, label) == QMessageBox::Yes;
}

void ViewUtil :: error (QWidget& parent, const QString& label) {
  QMessageBox::critical (&parent, QObject::tr ("Error"), label);
}

void ViewUtil :: about (QWidget& parent, const QString& label) {
  QMessageBox::about (&parent, QObject::tr ("About Dilay"), label);
}

void ViewUtil :: info (QWidget& parent, const QString& label) {
  QMessageBox::information (&parent, QObject::tr ("Information"), label);
}

#ifndef VIEW_UTIL
#define VIEW_UTIL

class QSpinBox;
class QDoubleSpinBox;
class QPushButton;
class QString;
class QToolButton;
class QRadioButton;
class QCheckBox;

namespace ViewUtil {
  QSpinBox*       spinBox     (int, int, int);
  QDoubleSpinBox* spinBox     (float, float, float, float);
  QPushButton*    pushButton  (const QString&, bool);
  QToolButton*    toolButton  (const QString&, bool);
  QRadioButton*   radioButton (const QString&, bool, bool = false);
  QCheckBox*      checkBox    (const QString&, bool, bool = false);
};

#endif

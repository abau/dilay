#ifndef VIEW_UTIL
#define VIEW_UTIL

class QSpinBox;
class QDoubleSpinBox;
class QPushButton;
class QString;
class QToolButton;

namespace ViewUtil {
  QSpinBox*       spinBox    (int, int, int);
  QDoubleSpinBox* spinBox    (float, float, float, float);
  QPushButton*    pushButton (const QString&);
  QToolButton*    toolButton (const QString&);
};

#endif

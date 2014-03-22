#ifndef VIEW_UTIL
#define VIEW_UTIL

class QSpinBox;
class QDoubleSpinBox;
class QPushButton;
class QString;

namespace ViewUtil {
  QSpinBox*       spinBox    (int, int, int);
  QDoubleSpinBox* spinBox    (float, float, float, float);
  QPushButton*    pushButton (const QString&);
};

#endif

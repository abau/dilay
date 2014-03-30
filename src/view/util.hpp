#ifndef VIEW_UTIL
#define VIEW_UTIL

#include <glm/fwd.hpp>
#include <functional>

class QSpinBox;
class QDoubleSpinBox;
class QPushButton;
class QString;
class QToolButton;
class QRadioButton;
class QCheckBox;
class QPoint;
class QMouseEvent;

namespace ViewUtil {
  QSpinBox*       spinBox     (int, int, int);
  QDoubleSpinBox* spinBox     (float, float, float, float);
  QPushButton*    pushButton  (const QString&, bool);
  QToolButton*    toolButton  (const QString&, bool);
  QRadioButton*   radioButton (const QString&, bool, bool = false);
  QCheckBox*      checkBox    (const QString&, bool, bool = false);
  glm::uvec2      toUVec2     (const QPoint&);
  glm::uvec2      toUVec2     (const QMouseEvent&);
  glm::ivec2      toIVec2     (const QPoint&);
  glm::ivec2      toIVec2     (const QMouseEvent&);
  void            connect     (QSpinBox*, const std::function <void (int)>&);
  void            connect     (QDoubleSpinBox*, const std::function <void (double)>&);
};

#endif

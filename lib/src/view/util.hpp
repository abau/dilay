#ifndef VIEW_UTIL
#define VIEW_UTIL

#include <functional>
#include <glm/fwd.hpp>

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
  QSpinBox&       spinBox     (int, int, int);
  QDoubleSpinBox& spinBox     (float, float, float, float);
  QPushButton&    pushButton  (const QString&, bool = false);
  QToolButton&    toolButton  (const QString&);
  QRadioButton&   radioButton (const QString&, bool = false);
  QCheckBox&      checkBox    (const QString&, bool = false);
  glm::uvec2      toUVec2     (const QPoint&);
  glm::uvec2      toUVec2     (const QMouseEvent&);
  glm::ivec2      toIVec2     (const QPoint&);
  glm::ivec2      toIVec2     (const QMouseEvent&);
  void            connect     (const QSpinBox&, const std::function <void (int)>&);
  void            connect     (const QDoubleSpinBox&, const std::function <void (double)>&);
  QWidget&        stretcher   (bool, bool);
};

#endif

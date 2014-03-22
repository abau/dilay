#ifndef DILAY_VIEW_PROPERTIES
#define DILAY_VIEW_PROPERTIES

#include <QToolBox>
#include "macro.hpp"

class ViewProperties : public QToolBox {
  public:
    DECLARE_BIG3 (ViewProperties)

  private:
    class Impl;
    Impl* impl;
};

#endif

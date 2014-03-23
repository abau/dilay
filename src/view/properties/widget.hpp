#ifndef DILAY_VIEW_PROPERTIES_WIDGET
#define DILAY_VIEW_PROPERTIES_WIDGET

#include <QWidget>
#include "macro.hpp"

class ViewPropertiesWidget : public QWidget {
  public:
    DECLARE_BIG3 (ViewPropertiesWidget)

  private:
    class Impl;
    Impl* impl;
};

#endif

#ifndef DILAY_VIEW_PROPERTIES_WIDGET
#define DILAY_VIEW_PROPERTIES_WIDGET

#include <QWidget>
#include "macro.hpp"

class ViewPropertiesSelection;

class ViewPropertiesWidget : public QWidget {
  public:
    DECLARE_BIG3 (ViewPropertiesWidget)

    ViewPropertiesSelection& selection ();

  private:
    IMPLEMENTATION
};

#endif

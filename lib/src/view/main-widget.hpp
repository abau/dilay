#ifndef DILAY_VIEW_MAIN_WIDGET
#define DILAY_VIEW_MAIN_WIDGET

#include <QSplitter>
#include "macro.hpp"

class Config;
class ViewMainWindow;
class ViewGlWidget;
class ViewPropertiesWidget;

class ViewMainWidget : public QSplitter {
  public:
    DECLARE_BIG3 (ViewMainWidget, ViewMainWindow&, Config&)

    ViewGlWidget&         glWidget    ();
    ViewPropertiesWidget& properties  ();

  private:
    IMPLEMENTATION
};

#endif

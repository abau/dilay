#ifndef DILAY_VIEW_MAIN_WIDGET
#define DILAY_VIEW_MAIN_WIDGET

#include <QSplitter>
#include "macro.hpp"

class ViewMainWindow;
class ViewGlWidget;
class ViewPropertiesWidget;

class ViewMainWidget : public QSplitter {
  public:
    DECLARE_BIG3 (ViewMainWidget, ViewMainWindow*)

    ViewGlWidget*         glWidget    ();
    ViewPropertiesWidget* properties  ();

  private:
    class Impl;
    Impl* impl;
};

#endif

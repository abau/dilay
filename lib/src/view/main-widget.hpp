#ifndef DILAY_VIEW_MAIN_WIDGET
#define DILAY_VIEW_MAIN_WIDGET

#include <QSplitter>
#include "macro.hpp"

class Cache;
class Config;
class ViewMainWindow;
class ViewGlWidget;
class ViewProperties;

class ViewMainWidget : public QSplitter {
  public:
    DECLARE_BIG2 (ViewMainWidget, ViewMainWindow&, Config&, Cache&)

    ViewGlWidget&   glWidget     ();
    ViewProperties& properties   ();
    void            deselectTool ();

  private:
    IMPLEMENTATION
};

#endif

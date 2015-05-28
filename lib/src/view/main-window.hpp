#ifndef DILAY_VIEW_MAIN_WINDOW
#define DILAY_VIEW_MAIN_WINDOW

#include <QMainWindow>
#include "macro.hpp"

class Cache;
class Config;
class ViewGlWidget;
class ViewProperties;
class ViewToolTip;

class ViewMainWindow : public QMainWindow {
    Q_OBJECT
  public:
    DECLARE_BIG2 (ViewMainWindow, Config&, Cache&)

    ViewGlWidget&   glWidget           ();
    ViewProperties& properties         ();
    void            deselectTool       ();
    void            showMessage        (const QString&);
    void            showToolTip        (const ViewToolTip&);
    void            showDefaultToolTip ();
    void            showNumFaces       (unsigned int);

  private:
    IMPLEMENTATION
};
#endif

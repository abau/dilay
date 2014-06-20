#ifndef DILAY_VIEW_MAIN_WINDOW
#define DILAY_VIEW_MAIN_WINDOW

#include <initializer_list>
#include <QMainWindow>
#include "macro.hpp"

class ViewGlWidget;
class ViewPropertiesWidget;

class ViewMainWindow : public QMainWindow {
    Q_OBJECT
  public:
    DECLARE_BIG3 (ViewMainWindow)

    ViewGlWidget&         glWidget           ();
    ViewPropertiesWidget& properties         ();
    void                  showMessage        (const QString&);
    void                  showDefaultMessage ();
    void                  showNumSelections  (unsigned int);

  private:
    class Impl;
    Impl* impl;
};
#endif

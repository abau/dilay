#ifndef DILAY_VIEW_PROPERTIES_WIDGET
#define DILAY_VIEW_PROPERTIES_WIDGET

#include <QStackedWidget>
#include "macro.hpp"

class QString;
class ViewProperties;
class ViewPropertiesSelection;

class ViewPropertiesWidget : public QStackedWidget {
  public:
    DECLARE_BIG2 (ViewPropertiesWidget)

    ViewPropertiesSelection& selection ();
    ViewProperties&          tool      ();
    void                     showTool  (const QString&);
    void                     resetTool ();

  private:
    IMPLEMENTATION
};

#endif

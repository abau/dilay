#ifndef DILAY_VIEW_PROPERTIES
#define DILAY_VIEW_PROPERTIES

#include <QWidget>
#include "macro.hpp"

class ViewPropertiesPart;

class ViewProperties : public QWidget {
  public:
    DECLARE_BIG3 (ViewProperties)

    void                label  (const QString&);
    void                reset  ();
    ViewPropertiesPart& header ();
    ViewPropertiesPart& body   ();
    ViewPropertiesPart& footer ();

  private:
    IMPLEMENTATION
};

#endif

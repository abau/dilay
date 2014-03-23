#ifndef DILAY_VIEW_PROPERTIES_BUTTON
#define DILAY_VIEW_PROPERTIES_BUTTON

#include <QToolButton>

class QString;
class QPaintEvent;

class ViewPropertiesButton : public QToolButton {
  Q_OBJECT
  public:
    ViewPropertiesButton (const QString&, bool = true);

  protected:
    void paintEvent (QPaintEvent*);

  signals:
    void collapse ();
    void expand   ();
};

#endif

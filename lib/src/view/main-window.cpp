#include <QLabel>
#include <QToolBar>
#include "view/main-widget.hpp"
#include "view/main-window.hpp"
#include "view/tool-tip.hpp"
#include "view/util.hpp"

struct ViewMainWindow :: Impl {
  ViewMainWindow* self;
  ViewMainWidget  mainWidget;
  QToolBar        statusBar;
  QLabel          messageLabel;

  Impl (ViewMainWindow* s, Config& config, Cache& cache) 
    : self       (s) 
    , mainWidget (*this->self, config, cache)
  {
    this->self->setCentralWidget (&this->mainWidget);
    this->self->addToolBar       (Qt::BottomToolBarArea, &this->statusBar);
    this->statusBar.setFloatable (false);
    this->statusBar.setMovable   (false);
    this->statusBar.addWidget    (new QLabel (" "));
    this->statusBar.addWidget    (&this->messageLabel);
    this->showDefaultToolTip     ();
  }

  ViewGlWidget&   glWidget     () { return this->mainWidget.glWidget     (); }
  ViewProperties& properties   () { return this->mainWidget.properties   (); }
  void            deselectTool () {        this->mainWidget.deselectTool (); }

  void showMessage (const QString& message) {
    this->messageLabel.setText (message);
  }

  void showToolTip (const ViewToolTip& tip) {
    this->showMessage (tip.toString ());
  }

  void showDefaultToolTip () {
    ViewToolTip tip;

    tip.add ( ViewToolTip::MouseEvent::Middle, QObject::tr ("Drag to rotate"));
    tip.add ( ViewToolTip::MouseEvent::Middle
            , ViewToolTip::Modifier::Ctrl, QObject::tr ("Gaze/Drag to pan"));

    this->showToolTip (tip);
  }
};

DELEGATE2_BIG2_SELF (ViewMainWindow, Config&, Cache&)
DELEGATE  (ViewGlWidget&  , ViewMainWindow, glWidget)
DELEGATE  (ViewProperties&, ViewMainWindow, properties)
DELEGATE  (void           , ViewMainWindow, deselectTool)
DELEGATE1 (void           , ViewMainWindow, showMessage, const QString&)
DELEGATE1 (void           , ViewMainWindow, showToolTip, const ViewToolTip&)
DELEGATE  (void           , ViewMainWindow, showDefaultToolTip)

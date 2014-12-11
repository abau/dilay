#include <QLabel>
#include <QToolBar>
#include "view/main-widget.hpp"
#include "view/main-window.hpp"
#include "view/tool/tip.hpp"
#include "view/util.hpp"

struct ViewMainWindow :: Impl {
  ViewMainWindow* self;
  ViewMainWidget& mainWidget;
  QToolBar&       statusBar;
  QLabel&         messageLabel;
  QLabel&         numSelectionsLabel;

  Impl (ViewMainWindow* s) 
    : self               (s) 
    , mainWidget         (*new ViewMainWidget (*this->self))
    , statusBar          (*new QToolBar       ())
    , messageLabel       (*new QLabel         ())
    , numSelectionsLabel (*new QLabel         ())
  {
    this->self->setCentralWidget       (&this->mainWidget);
    this->self->addToolBar             (Qt::BottomToolBarArea, &this->statusBar);
    this->statusBar.setFloatable       (false);
    this->statusBar.setMovable         (false);
    this->statusBar.addWidget          (new QLabel (" "));
    this->statusBar.addWidget          (&this->messageLabel);
    this->statusBar.addWidget          (&ViewUtil::stretcher (true,false));
    this->statusBar.addWidget          (&this->numSelectionsLabel);
    this->statusBar.addWidget          (new QLabel (" "));
    this->showDefaultToolTip           ();
    this->showNumSelections            (0);
  }

  ViewGlWidget&         glWidget   () { return this->mainWidget.glWidget   (); }
  ViewPropertiesWidget& properties () { return this->mainWidget.properties (); }

  void showMessage (const QString& message) {
    this->messageLabel.setText (message);
  }

  void showToolTip (const ViewToolTip& tip) {
    this->showMessage (tip.toString ());
  }

  void showDefaultToolTip () {
    ViewToolTip tip;

    tip.add ( ViewToolTip::Button::Left, QObject::tr ("Select"));
    tip.add ( ViewToolTip::Button::Middle, QObject::tr ("Drag To Rotate"));
    tip.add ( ViewToolTip::Button::Middle
            , ViewToolTip::Modifier::Ctrl, QObject::tr ("Drag To Pan"));
    tip.add ( ViewToolTip::Button::Middle
            , ViewToolTip::Modifier::Shift, QObject::tr ("Gaze"));
    tip.add ( ViewToolTip::Button::Right, QObject::tr ("Menu"));

    this->showToolTip (tip);
  }

  void showNumSelections (unsigned int n) {
    this->numSelectionsLabel.setText ( QObject::tr            ("Selections") 
                                     + QString::fromStdString (": " + std::to_string (n)));
  }
};

DELEGATE_BIG3_SELF (ViewMainWindow)
DELEGATE  (ViewGlWidget&        , ViewMainWindow, glWidget)
DELEGATE  (ViewPropertiesWidget&, ViewMainWindow, properties)
DELEGATE1 (void                 , ViewMainWindow, showMessage, const QString&)
DELEGATE1 (void                 , ViewMainWindow, showToolTip, const ViewToolTip&)
DELEGATE  (void                 , ViewMainWindow, showDefaultToolTip)
DELEGATE1 (void                 , ViewMainWindow, showNumSelections, unsigned int)

/* This file is part of Dilay
 * Copyright © 2015 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <QApplication>
#include <QLabel>
#include <QMenu>
#include <QShortcut>
#include <QStatusBar>
#include "scene.hpp"
#include "state.hpp"
#include "tool/move-camera.hpp"
#include "view/gl-widget.hpp"
#include "view/main-widget.hpp"
#include "view/main-window.hpp"
#include "view/menu-bar.hpp"
#include "view/tool-tip.hpp"

struct ViewMainWindow :: Impl {
  ViewMainWindow* self;
  ViewMainWidget& mainWidget;
  QStatusBar&     statusBar;
  QLabel&         messageLabel;
  QLabel&         numFacesLabel;

  Impl (ViewMainWindow* s, Config& config, Cache& cache) 
    : self          (s) 
    , mainWidget    (*new ViewMainWidget (*this->self, config, cache))
    , statusBar     (*new QStatusBar)
    , messageLabel  (*new QLabel)
    , numFacesLabel (*new QLabel)
  {
    this->self->setCentralWidget (&this->mainWidget);

    ViewMenuBar::setup (*this->self, this->mainWidget.glWidget ());

    this->setupShortcuts ();
    this->setupStatusBar ();
  }

  void setupShortcuts () {
    auto addShortcut = [this] (const QKeySequence& keySequence, const std::function <void ()>& f) {
      QShortcut* s = new QShortcut (keySequence, this->self);

      QObject::connect (s, &QShortcut::activated, f);
    };

    addShortcut (Qt::Key_Backspace, [this] () {
      if (this->mainWidget.glWidget ().state ().hasTool ()) {
        this->mainWidget.glWidget ().state ().resetTool ();
      }
#ifndef NDEBUG
      else {
        QApplication::quit ();
      }
#endif
    });
#ifndef NDEBUG
    addShortcut (Qt::Key_I, [this] () {
      this->mainWidget.glWidget ().state ().scene ().printStatistics (false);
    });
    addShortcut (Qt::SHIFT + Qt::Key_I, [this] () {
      this->mainWidget.glWidget ().state ().scene ().printStatistics (true);
    });
#endif
    addShortcut (Qt::SHIFT + Qt::Key_C, [this] () {
      this->mainWidget.glWidget ()
                      .toolMoveCamera ()
                      .snap (this->mainWidget.glWidget ().state (), true);
    });
  }

  void setupStatusBar () {
    this->self->setStatusBar (&this->statusBar);

    this->statusBar.setStyleSheet      ("QStatusBar::item { border: 0px solid black };");
    this->statusBar.setSizeGripEnabled (false);
    this->statusBar.addPermanentWidget (&this->messageLabel,1);
    this->statusBar.addPermanentWidget (&this->numFacesLabel);

    this->showDefaultToolTip ();
    this->showNumFaces       (0);
  }

  void showMessage (const QString& message) {
    this->messageLabel.setText (message);
  }

  void showToolTip (const ViewToolTip& tip) {
    this->showMessage (tip.toString ());
  }

  void showDefaultToolTip () {
    ViewToolTip tip;

    tip.add ( ViewToolTip::MouseEvent::Middle, QObject::tr ("Drag to rotate"));
    tip.add ( ViewToolTip::MouseEvent::Middle, ViewToolTip::Modifier::Shift
            , QObject::tr ("Drag to move"));
    tip.add ( ViewToolTip::MouseEvent::Middle, ViewToolTip::Modifier::Ctrl
            , QObject::tr ("Gaze"));

    this->showToolTip (tip);
  }

  void showNumFaces (unsigned int n) {
    this->numFacesLabel.setText (QString::number (n).append (" faces"));
  }
};

DELEGATE2_BIG2_SELF (ViewMainWindow, Config&, Cache&)
GETTER    (ViewMainWidget&, ViewMainWindow, mainWidget)
DELEGATE1 (void           , ViewMainWindow, showMessage, const QString&)
DELEGATE1 (void           , ViewMainWindow, showToolTip, const ViewToolTip&)
DELEGATE  (void           , ViewMainWindow, showDefaultToolTip)
DELEGATE1 (void           , ViewMainWindow, showNumFaces, unsigned int)

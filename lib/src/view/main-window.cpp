/* This file is part of Dilay
 * Copyright © 2015-2017 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <QCloseEvent>
#include <QLabel>
#include <QShortcut>
#include <QStatusBar>
#include "state.hpp"
#include "tool/move-camera.hpp"
#include "view/gl-widget.hpp"
#include "view/info-pane.hpp"
#include "view/main-window.hpp"
#include "view/menu-bar.hpp"
#include "view/tool-pane.hpp"
#include "view/tool-tip.hpp"
#include "view/util.hpp"

struct ViewMainWindow::Impl
{
  ViewMainWindow* self;
  ViewGlWidget&   glWidget;
  ViewToolPane&   toolPane;
  ViewInfoPane&   infoPane;

  Impl (ViewMainWindow* s, Config& config, Cache& cache)
    : self (s)
    , glWidget (*new ViewGlWidget (*this->self, config, cache))
    , toolPane (*new ViewToolPane (this->glWidget))
    , infoPane (*new ViewInfoPane (this->glWidget))
  {
    this->self->setCentralWidget (&this->glWidget);
    this->self->addDockWidget (Qt::LeftDockWidgetArea, &this->toolPane);
    this->self->addDockWidget (Qt::RightDockWidgetArea, &this->infoPane);

    ViewMenuBar::setup (*this->self, this->glWidget);

    this->setupShortcuts ();
  }

  void setupShortcuts ()
  {
    auto addShortcut = [this](const QKeySequence& keySequence, const std::function<void()>& f) {
      QShortcut* s = new QShortcut (keySequence, this->self);

      QObject::connect (s, &QShortcut::activated, f);
    };

    addShortcut (Qt::Key_Escape, [this]() {
      if (this->glWidget.state ().hasTool ())
      {
        this->glWidget.state ().resetTool ();
      }
#ifndef NDEBUG
      else
      {
        this->self->close ();
      }
#endif
    });
    addShortcut (Qt::SHIFT + Qt::Key_C, [this]() {
      this->glWidget.toolMoveCamera ().snap (this->glWidget.state (), true);
    });
  }

  void update ()
  {
    this->self->QMainWindow::update ();
    this->glWidget.update ();
  }

  void closeEvent (QCloseEvent* e)
  {
#ifndef NDEBUG
    e->accept ();
#else
    if (ViewUtil::question (*this->self, QObject::tr ("Do you want to quit?")))
    {
      e->accept ();
    }
    else
    {
      e->ignore ();
    }
#endif
  }
};

DELEGATE2_BIG2_SELF (ViewMainWindow, Config&, Cache&)
GETTER (ViewGlWidget&, ViewMainWindow, glWidget)
GETTER (ViewToolPane&, ViewMainWindow, toolPane)
GETTER (ViewInfoPane&, ViewMainWindow, infoPane)
DELEGATE (void, ViewMainWindow, update)
DELEGATE1 (void, ViewMainWindow, closeEvent, QCloseEvent*)

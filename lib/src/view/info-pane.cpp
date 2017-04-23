/* This file is part of Dilay
 * Copyright © 2015-2017 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <QScrollArea>
#include <QTabWidget>
#include <QVBoxLayout>
#include <vector>
#include "state.hpp"
#include "view/gl-widget.hpp"
#include "view/info-pane.hpp"
#include "view/tool-tip.hpp"
#include "view/two-column-grid.hpp"
#include "view/util.hpp"

struct ViewInfoPane::Impl
{
  ViewInfoPane*      self;
  ViewGlWidget&      glWidget;
  ViewTwoColumnGrid& keys;

  Impl (ViewInfoPane* s, ViewGlWidget& g)
    : self (s)
    , glWidget (g)
    , keys (*new ViewTwoColumnGrid)
  {
    QScrollArea* scrollArea = new QScrollArea;
    QTabWidget*  tabWidget = new QTabWidget;

    tabWidget->addTab (this->initializeKeysTab (), QObject::tr ("Keys"));

    scrollArea->setWidgetResizable (true);
    scrollArea->setWidget (tabWidget);

    this->self->setWindowTitle (QObject::tr ("Info"));
    this->self->setWidget (scrollArea);
    this->self->setFeatures (DockWidgetMovable | DockWidgetClosable);
    this->self->setAllowedAreas (Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
  }

  QWidget* initializeKeysTab ()
  {
    this->keys.setEqualColumnStretch ();
    this->showDefaultToolTip ();

    QWidget*     pane = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout (pane);

    layout->setContentsMargins (0, 0, 0, 0);
    layout->setSpacing (0);
    layout->addWidget (&this->keys);
    layout->addStretch (1);

    return pane;
  }

  void showToolTip (const ViewToolTip& tip)
  {
    this->reset ();
    this->addToolTip (tip);
    this->addDefaultToolTip (true);
  }

  void addToolTip (const ViewToolTip& tip)
  {
    tip.render (
      [this](const QString& action, const QString& tip) { this->keys.add (action, tip); });
    this->self->layout ()->update ();
  }

  void showDefaultToolTip ()
  {
    this->reset ();
    this->addDefaultToolTip (false);
  }

  void addDefaultToolTip (bool addSeparator)
  {
    if (addSeparator)
    {
      this->keys.addSeparator ();
    }

    ViewToolTip tip;

    tip.add (ViewToolTip::MouseEvent::Middle, QObject::tr ("Drag to rotate"));
    tip.add (ViewToolTip::MouseEvent::Middle, ViewToolTip::Modifier::Shift,
             QObject::tr ("Drag to move"));
    tip.add (ViewToolTip::MouseEvent::Middle, ViewToolTip::Modifier::Ctrl, QObject::tr ("Gaze"));

    this->addToolTip (tip);
  }

  void showNumFaces (unsigned int)
  {
  }

  void reset ()
  {
    this->keys.reset ();
  }
};

DELEGATE_BIG2_BASE (ViewInfoPane, (ViewGlWidget & g, QWidget* p), (this, g), QDockWidget, (p))
DELEGATE1 (void, ViewInfoPane, showToolTip, const ViewToolTip&)
DELEGATE1 (void, ViewInfoPane, addToolTip, const ViewToolTip&)
DELEGATE (void, ViewInfoPane, showDefaultToolTip)
DELEGATE1 (void, ViewInfoPane, showNumFaces, unsigned int)
DELEGATE (void, ViewInfoPane, reset)

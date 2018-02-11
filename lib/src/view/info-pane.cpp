/* This file is part of Dilay
 * Copyright © 2015-2018 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <QScrollArea>
#include <QTabWidget>
#include <QVBoxLayout>
#include "view/info-pane.hpp"
#include "view/info-pane/scene.hpp"
#include "view/tool-tip.hpp"
#include "view/two-column-grid.hpp"

struct ViewInfoPane::Impl
{
  ViewInfoPane*      self;
  ViewGlWidget&      glWidget;
  ViewTwoColumnGrid& toolTip;
  ViewInfoPaneScene& scene;

  Impl (ViewInfoPane* s, ViewGlWidget& g)
    : self (s)
    , glWidget (g)
    , toolTip (*new ViewTwoColumnGrid)
    , scene (*new ViewInfoPaneScene (g))
  {
    QScrollArea* scrollArea = new QScrollArea;
    QTabWidget*  tabWidget = new QTabWidget;

    tabWidget->addTab (this->initializeToolTipTab (), QObject::tr ("Keys"));
    tabWidget->addTab (&this->scene, QObject::tr ("Scene"));

    scrollArea->setWidgetResizable (true);
    scrollArea->setWidget (tabWidget);

    this->self->setWindowTitle (QObject::tr ("Info"));
    this->self->setWidget (scrollArea);
    this->self->setFeatures (DockWidgetMovable | DockWidgetClosable);
    this->self->setAllowedAreas (Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

    this->resetToolTip ();
  }

  QWidget* initializeToolTipTab ()
  {
    ViewToolTip dummyTip;
    dummyTip.add (ViewInput::Event::MouseMiddle, ViewInput::Modifier::Shift,
                  "01234567890123456789");
    this->addToolTip (dummyTip);
    this->toolTip.setEqualColumnStretch ();

    QWidget*     pane = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout (pane);

    layout->setContentsMargins (0, 0, 0, 0);
    layout->setSpacing (0);
    layout->addWidget (&this->toolTip);
    layout->addStretch (1);

    return pane;
  }

  void addToolTip (const ViewToolTip& tip)
  {
    if (tip.isEmpty ())
    {
      return;
    }
    else if (this->toolTip.numRows () > 0)
    {
      this->toolTip.addSeparator ();
    }
    tip.render (
      [this](const QString& action, const QString& tip) { this->toolTip.add (action, tip); });
  }

  void resetToolTip () { this->toolTip.reset (); }
};

DELEGATE_BIG2_BASE (ViewInfoPane, (ViewGlWidget & g, QWidget* p), (this, g), QDockWidget, (p))
GETTER (ViewInfoPaneScene&, ViewInfoPane, scene)
DELEGATE1 (void, ViewInfoPane, addToolTip, const ViewToolTip&)
DELEGATE (void, ViewInfoPane, resetToolTip)

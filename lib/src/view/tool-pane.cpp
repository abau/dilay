/* This file is part of Dilay
 * Copyright © 2015-2017 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <QPushButton>
#include <QScrollArea>
#include <QTabWidget>
#include <QVBoxLayout>
#include <unordered_map>
#include "state.hpp"
#include "tools.hpp"
#include "view/gl-widget.hpp"
#include "view/tool-pane.hpp"
#include "view/two-column-grid.hpp"
#include "view/util.hpp"

struct ViewToolPane::Impl
{
  typedef std::unordered_map<const char*, QPushButton*> Buttons;

  ViewToolPane*      self;
  ViewGlWidget&      glWidget;
  QVBoxLayout&       layout;
  QTabWidget&        toolSelection;
  ViewTwoColumnGrid& properties;
  Buttons            buttons;

  Impl (ViewToolPane* s, ViewGlWidget& g)
    : self (s)
    , glWidget (g)
    , layout (*new QVBoxLayout)
    , toolSelection (*new QTabWidget)
    , properties (*new ViewTwoColumnGrid)
  {
    QScrollArea* scrollArea = new QScrollArea;
    QWidget*     pane = new QWidget;

    this->initializeToolSelection ();

    this->layout.setContentsMargins (0, 0, 0, 0);
    this->layout.setSpacing (0);
    this->layout.addWidget (&this->toolSelection);
    this->layout.addWidget (&this->properties);
    this->layout.addStretch (1);

    scrollArea->setWidgetResizable (true);
    scrollArea->setWidget (pane);
    pane->setLayout (&this->layout);

    this->self->setWindowTitle (QObject::tr ("Tools"));
    this->self->setWidget (scrollArea);
    this->self->setFeatures (DockWidgetMovable);
    this->self->setAllowedAreas (Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
  }

  void initializeToolSelection ()
  {
    this->toolSelection.addTab (this->initalizeSculptSelection (), QObject::tr ("Sculpt"));
    this->toolSelection.addTab (this->initalizeSketchSelection (), QObject::tr ("Sketch"));

    QObject::connect (&this->toolSelection, &QTabWidget::currentChanged, [this](int) {
      this->glWidget.state ().resetTool ();
      ViewUtil::adjustSize (this->toolSelection);
    });
  }

  template <typename T> void addToolButton (QLayout* layout, const QString& name)
  {
    QPushButton& button = ViewUtil::pushButton (name);
    this->buttons.emplace (T::classKey (), &button);
    button.setCheckable (true);

    ViewUtil::connect (button, [this, &button]() {
      State& s = this->glWidget.state ();
      s.resetTool ();
      s.setTool (std::move (*new T (s)));
    });
    layout->addWidget (&button);
  }

  QWidget* initalizeSculptSelection ()
  {
    QWidget*     toolPane = new QWidget;
    QVBoxLayout* toolPaneLayout = new QVBoxLayout;

    toolPane->setLayout (toolPaneLayout);
    this->addToolButton<ToolNewMesh> (toolPaneLayout, QObject::tr ("New mesh"));
    this->addToolButton<ToolDeleteMesh> (toolPaneLayout, QObject::tr ("Delete mesh"));
    this->addToolButton<ToolMoveMesh> (toolPaneLayout, QObject::tr ("Move mesh"));
    toolPaneLayout->addWidget (&ViewUtil::horizontalLine ());
    this->addToolButton<ToolSculptDraw> (toolPaneLayout, QObject::tr ("Draw"));
    this->addToolButton<ToolSculptCrease> (toolPaneLayout, QObject::tr ("Crease"));
    this->addToolButton<ToolSculptGrab> (toolPaneLayout, QObject::tr ("Grab"));
    this->addToolButton<ToolSculptFlatten> (toolPaneLayout, QObject::tr ("Flatten"));
    this->addToolButton<ToolSculptSmooth> (toolPaneLayout, QObject::tr ("Smooth"));
    this->addToolButton<ToolSculptPinch> (toolPaneLayout, QObject::tr ("Pinch"));
    this->addToolButton<ToolSculptReduce> (toolPaneLayout, QObject::tr ("Reduce"));
    this->addToolButton<ToolTrimMesh> (toolPaneLayout, QObject::tr ("Trim"));

    toolPaneLayout->addStretch (1);

    return toolPane;
  }

  QWidget* initalizeSketchSelection ()
  {
    QWidget*     toolPane = new QWidget;
    QVBoxLayout* toolPaneLayout = new QVBoxLayout;

    toolPane->setLayout (toolPaneLayout);
    this->addToolButton<ToolNewSketch> (toolPaneLayout, QObject::tr ("New sketch"));
    this->addToolButton<ToolModifySketch> (toolPaneLayout, QObject::tr ("Modify sketch"));
    this->addToolButton<ToolDeleteSketch> (toolPaneLayout, QObject::tr ("Delete sketch"));
    this->addToolButton<ToolRebalanceSketch> (toolPaneLayout, QObject::tr ("Rebalance sketch"));
    toolPaneLayout->addWidget (&ViewUtil::horizontalLine ());
    this->addToolButton<ToolSketchSpheres> (toolPaneLayout, QObject::tr ("Sketch spheres"));
    toolPaneLayout->addWidget (&ViewUtil::horizontalLine ());
    this->addToolButton<ToolConvertSketch> (toolPaneLayout, QObject::tr ("Convert sketch"));

    toolPaneLayout->addStretch (1);

    return toolPane;
  }

  ViewToolPaneSelection selection () const
  {
    switch (this->toolSelection.currentIndex ())
    {
      case 0:
        return ViewToolPaneSelection::Sculpt;
      case 1:
        return ViewToolPaneSelection::Sketch;
      default:
        DILAY_IMPOSSIBLE
    }
  }

  QPushButton& button (const char* key)
  {
    auto it = this->buttons.find (key);
    assert (it != this->buttons.end ());
    return *it->second;
  }
};

DELEGATE_BIG2_BASE (ViewToolPane, (ViewGlWidget & g, QWidget* p), (this, g), QDockWidget, (p))
GETTER (ViewTwoColumnGrid&, ViewToolPane, properties)
DELEGATE_CONST (ViewToolPaneSelection, ViewToolPane, selection)
DELEGATE1 (QPushButton&, ViewToolPane, button, const char*)

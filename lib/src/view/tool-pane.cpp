/* This file is part of Dilay
 * Copyright © 2015,2016 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <QPushButton>
#include <QScrollArea>
#include <QTabWidget>
#include <QVBoxLayout>
#include "state.hpp"
#include "tools.hpp"
#include "view/gl-widget.hpp"
#include "view/tool-pane.hpp"
#include "view/two-column-grid.hpp"
#include "view/util.hpp"

struct ViewToolPane::Impl {
  ViewToolPane*              self;
  ViewGlWidget&              glWidget;
  QVBoxLayout&               layout;
  ViewTwoColumnGrid*         properties;
  std::vector <QPushButton*> toolButtons;

  Impl (ViewToolPane* s, ViewGlWidget& g)
    : self       (s)
    , glWidget   (g)
    , layout     (*new QVBoxLayout)
    , properties (nullptr)
  {
    QScrollArea* scrollArea = new QScrollArea;
    QWidget*     pane       = new QWidget;

    scrollArea->setWidgetResizable (true);
    scrollArea->setWidget (pane);
    pane->setLayout (&this->layout);

    this->layout.setSpacing (0);
    this->layout.addWidget  (this->initializeToolSelection ());
    this->layout.addStretch (1);

    this->self->setWidget       (scrollArea);
    this->self->setFeatures     (DockWidgetMovable);
    this->self->setAllowedAreas (Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
  }

  ViewTwoColumnGrid& makeProperties () {
    this->resetProperties ();

    assert (this->layout.count () == 2);

    this->properties = new ViewTwoColumnGrid;
    this->layout.insertWidget (1, this->properties);
    return *this->properties;
  }

  void resetProperties () {
    if (this->properties) {
      this->layout.removeWidget (this->properties);
      delete this->properties;
      this->properties = nullptr;
    }
  }

  QWidget* initializeToolSelection () {
    QTabWidget* toolPane = new QTabWidget;
    toolPane->addTab (this->initalizeSculptSelection (), QObject::tr ("Sculpt"));
    toolPane->addTab (this->initalizeSketchSelection (), QObject::tr ("Sketch"));

    QObject::connect (toolPane, &QTabWidget::currentChanged, [this] (int) {
      this->glWidget.state ().resetTool (true);
    });
    return toolPane;
  }

  QWidget* initalizeSculptSelection () {
    QWidget*     toolPane       = new QWidget;
    QVBoxLayout* toolPaneLayout = new QVBoxLayout;

    toolPane->setLayout (toolPaneLayout);
    this->addToolButton <ToolNewMesh>       (toolPaneLayout, QObject::tr ("New mesh"));
    this->addToolButton <ToolDeleteMesh>    (toolPaneLayout, QObject::tr ("Delete mesh"));
    this->addToolButton <ToolMoveMesh>      (toolPaneLayout, QObject::tr ("Move mesh"));
    toolPaneLayout->addWidget (&ViewUtil::horizontalLine ());
    this->addToolButton <ToolSculptDraw>    (toolPaneLayout, QObject::tr ("Draw"));
    this->addToolButton <ToolSculptCrease>  (toolPaneLayout, QObject::tr ("Crease"));
    this->addToolButton <ToolSculptGrab>    (toolPaneLayout, QObject::tr ("Grab"));
    this->addToolButton <ToolSculptDrag>    (toolPaneLayout, QObject::tr ("Drag"));
    this->addToolButton <ToolSculptFlatten> (toolPaneLayout, QObject::tr ("Flatten"));
    this->addToolButton <ToolSculptSmooth>  (toolPaneLayout, QObject::tr ("Smooth"));
    this->addToolButton <ToolSculptPinch>   (toolPaneLayout, QObject::tr ("Pinch"));
    this->addToolButton <ToolSculptReduce>  (toolPaneLayout, QObject::tr ("Reduce"));

    toolPaneLayout->addStretch (1);

    return toolPane;
  }

  QWidget* initalizeSketchSelection () {
    QWidget*     toolPane       = new QWidget;
    QVBoxLayout* toolPaneLayout = new QVBoxLayout;

    toolPane->setLayout (toolPaneLayout);
    this->addToolButton <ToolNewSketch>        (toolPaneLayout, QObject::tr ("New sketch"));
    this->addToolButton <ToolModifySketch>     (toolPaneLayout, QObject::tr ("Modify sketch"));
    this->addToolButton <ToolDeleteSketch>     (toolPaneLayout, QObject::tr ("Delete sketch"));
    this->addToolButton <ToolRebalanceSketch>  (toolPaneLayout, QObject::tr ("Rebalance sketch"));
    toolPaneLayout->addWidget (&ViewUtil::horizontalLine ());
    this->addToolButton <ToolSketchSpheres>    (toolPaneLayout, QObject::tr ("Sketch spheres"));
    toolPaneLayout->addWidget (&ViewUtil::horizontalLine ());
    this->addToolButton <ToolConvertSketch>    (toolPaneLayout, QObject::tr ("Convert sketch"));

    toolPaneLayout->addStretch (1);

    return toolPane;
  }

  template <typename T>
  void addToolButton (QLayout* layout, const QString& name) {
    QPushButton& button = ViewUtil::pushButton (name);
    button.setCheckable (true);
    
    ViewUtil::connect (button, [this, &button] () {
      for (QPushButton* b : this->toolButtons) {
        b->setChecked (b == &button);
      }

      State& s = this->glWidget.state ();
      s.resetTool (false);
      s.setTool   (std::move (*new T (s)));
    });
    layout->addWidget           (&button);
    this->toolButtons.push_back (&button);
  }

  void deselectTool () {
    assert (this->glWidget.state ().hasTool () == false);

    for (QPushButton* b : this->toolButtons) {
      b->setChecked (false);
    }
  }
};

DELEGATE_BIG2_BASE (ViewToolPane, (ViewGlWidget& g, QWidget* p), (this, g), QDockWidget, (p))
DELEGATE (ViewTwoColumnGrid&, ViewToolPane, makeProperties)
DELEGATE (void,               ViewToolPane, resetProperties)
DELEGATE (void,               ViewToolPane, deselectTool)

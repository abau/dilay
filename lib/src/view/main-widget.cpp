/* This file is part of Dilay
 * Copyright © 2015 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <QPushButton>
#include <QTabWidget>
#include <QVBoxLayout>
#include "state.hpp"
#include "tools.hpp"
#include "view/gl-widget.hpp"
#include "view/main-widget.hpp"
#include "view/properties.hpp"
#include "view/util.hpp"

struct ViewMainWidget :: Impl {
  ViewMainWidget*            self;
  ViewGlWidget&              glWidget;
  ViewProperties&            properties;
  std::vector <QPushButton*> toolButtons;

  Impl (ViewMainWidget* s, ViewMainWindow& mW, Config& config, Cache& cache) 
    : self       (s) 
    , glWidget   (*new ViewGlWidget (mW, config, cache))
    , properties (*new ViewProperties)
  {
    this->self->addWidget (this->initalizeLeftPane ());
    this->self->addWidget (&this->glWidget);
  }

  QWidget* initalizeLeftPane () {
    QWidget*     leftPane       = new QWidget;
    QVBoxLayout* leftPaneLayout = new QVBoxLayout;

    leftPane->setLayout (leftPaneLayout);
    leftPaneLayout->setSpacing (0);
    leftPaneLayout->addWidget  (this->initalizeToolPane ());
    leftPaneLayout->addWidget  (&this->properties);
    leftPaneLayout->addStretch (1);

    return leftPane;
  }

  QWidget* initalizeToolPane () {
    QTabWidget* toolPane = new QTabWidget;
    toolPane->addTab (this->initalizeSculptToolPane (), QObject::tr ("Sculpt"));
    toolPane->addTab (this->initalizeSketchToolPane (), QObject::tr ("Sketch"));

    QObject::connect (toolPane, &QTabWidget::currentChanged, [this] (int) {
      this->glWidget.state ().resetTool ();
    });
    return toolPane;
  }

  QWidget* initalizeSculptToolPane () {
    QWidget*     toolPane       = new QWidget;
    QVBoxLayout* toolPaneLayout = new QVBoxLayout;

    toolPane->setLayout (toolPaneLayout);
    this->addToolButton <ToolNewMesh>       (toolPaneLayout, QObject::tr ("New mesh"));
    this->addToolButton <ToolDeleteMesh>    (toolPaneLayout, QObject::tr ("Delete mesh"));
    this->addToolButton <ToolMoveMesh>      (toolPaneLayout, QObject::tr ("Move mesh"));
    this->addToolButton <ToolSculptCarve>   (toolPaneLayout, QObject::tr ("Carve"));
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

  QWidget* initalizeSketchToolPane () {
    QWidget*     toolPane       = new QWidget;
    QVBoxLayout* toolPaneLayout = new QVBoxLayout;

    toolPane->setLayout (toolPaneLayout);
    this->addToolButton <ToolNewSketch>    (toolPaneLayout, QObject::tr ("New sketch"));
    this->addToolButton <ToolModifySketch> (toolPaneLayout, QObject::tr ("Modify sketch"));

    toolPaneLayout->addStretch (1);
    return toolPane;
  }

  template <typename T>
  void addToolButton (QLayout* layout, const QString& name) {
    QPushButton& button = ViewUtil::pushButton (name);
    button.setCheckable (true);
    
    ViewUtil::connect (button, [this, &button] () {
      this->selectOnly (button);

      State& s = this->glWidget.state ();
      s.resetTool (false);
      s.setTool   (std::move (*new T (s)));
    });
    layout->addWidget           (&button);
    this->toolButtons.push_back (&button);
  }

  void selectOnly (QPushButton& button) {
    for (QPushButton* b : this->toolButtons) {
      b->setChecked (b == &button);
    }
  }

  void deselectTool () {
    assert (this->glWidget.state ().hasTool () == false);
    for (QPushButton* b : this->toolButtons) {
      b->setChecked (false);
    }
  }
};

DELEGATE3_BIG2_SELF (ViewMainWidget, ViewMainWindow&, Config&, Cache&)
GETTER   (ViewGlWidget&  , ViewMainWidget, glWidget)
GETTER   (ViewProperties&, ViewMainWidget, properties)
DELEGATE (void           , ViewMainWidget, deselectTool)

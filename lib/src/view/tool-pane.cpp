/* This file is part of Dilay
 * Copyright © 2015-2018 Alexander Bau
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
  typedef std::unordered_map<ToolKey, QPushButton*> Buttons;

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

  void addToolButton (ToolKey key, QLayout* layout, const QString& name)
  {
    QPushButton& button = ViewUtil::pushButton (name);
    this->buttons.emplace (key, &button);
    button.setCheckable (true);

    ViewUtil::connect (button, [this, key]() { this->glWidget.state ().setTool (key); });
    layout->addWidget (&button);
  }

  QWidget* initalizeSculptSelection ()
  {
    QWidget*     toolPane = new QWidget;
    QVBoxLayout* toolPaneLayout = new QVBoxLayout;

    toolPane->setLayout (toolPaneLayout);
    this->addToolButton (ToolKey::NewMesh, toolPaneLayout, QObject::tr ("01234567890123456789"));
    this->addToolButton (ToolKey::DeleteMesh, toolPaneLayout, QObject::tr ("Delete mesh"));
    this->addToolButton (ToolKey::MoveMesh, toolPaneLayout, QObject::tr ("Move mesh"));
    this->addToolButton (ToolKey::RotateMesh, toolPaneLayout, QObject::tr ("Rotate mesh"));
    toolPaneLayout->addWidget (&ViewUtil::horizontalLine ());
    this->addToolButton (ToolKey::SculptDraw, toolPaneLayout, QObject::tr ("Draw"));
    this->addToolButton (ToolKey::SculptCrease, toolPaneLayout, QObject::tr ("Crease"));
    this->addToolButton (ToolKey::SculptGrab, toolPaneLayout, QObject::tr ("Grab"));
    this->addToolButton (ToolKey::SculptPinch, toolPaneLayout, QObject::tr ("Pinch"));
    toolPaneLayout->addWidget (&ViewUtil::horizontalLine ());
    this->addToolButton (ToolKey::SculptSmooth, toolPaneLayout, QObject::tr ("Smooth"));
    this->addToolButton (ToolKey::SculptFlatten, toolPaneLayout, QObject::tr ("Flatten"));
    this->addToolButton (ToolKey::SculptReduce, toolPaneLayout, QObject::tr ("Reduce"));
    toolPaneLayout->addWidget (&ViewUtil::horizontalLine ());
    this->addToolButton (ToolKey::Remesh, toolPaneLayout, QObject::tr ("Remesh"));
    this->addToolButton (ToolKey::TrimMesh, toolPaneLayout, QObject::tr ("Trim"));

    toolPaneLayout->addStretch (1);

    return toolPane;
  }

  QWidget* initalizeSketchSelection ()
  {
    QWidget*     toolPane = new QWidget;
    QVBoxLayout* toolPaneLayout = new QVBoxLayout;

    toolPane->setLayout (toolPaneLayout);
    this->addToolButton (ToolKey::EditSketch, toolPaneLayout, QObject::tr ("Edit sketch"));
    this->addToolButton (ToolKey::DeleteSketch, toolPaneLayout, QObject::tr ("Delete sketch"));
    this->addToolButton (ToolKey::RebalanceSketch, toolPaneLayout,
                         QObject::tr ("Rebalance sketch"));
    toolPaneLayout->addWidget (&ViewUtil::horizontalLine ());
    this->addToolButton (ToolKey::SketchSpheres, toolPaneLayout, QObject::tr ("Sketch spheres"));
    toolPaneLayout->addWidget (&ViewUtil::horizontalLine ());
    this->addToolButton (ToolKey::ConvertSketch, toolPaneLayout, QObject::tr ("Convert sketch"));

    toolPaneLayout->addStretch (1);

    return toolPane;
  }

  void forceWidth ()
  {
    const auto it = this->buttons.find (ToolKey::NewMesh);
    assert (it != this->buttons.end ());

    it->second->setText (QObject::tr ("New mesh"));
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

  void setButtonState (ToolKey key, bool state)
  {
    auto it = this->buttons.find (key);
    assert (it != this->buttons.end ());
    it->second->setChecked (state);
  }

  QString buttonText (ToolKey key) const
  {
    auto it = this->buttons.find (key);
    assert (it != this->buttons.end ());
    return it->second->text ();
  }
};

DELEGATE_BIG2_BASE (ViewToolPane, (ViewGlWidget & g, QWidget* p), (this, g), QDockWidget, (p))
GETTER (ViewTwoColumnGrid&, ViewToolPane, properties)
DELEGATE (void, ViewToolPane, forceWidth)
DELEGATE_CONST (ViewToolPaneSelection, ViewToolPane, selection)
DELEGATE2 (void, ViewToolPane, setButtonState, ToolKey, bool)
DELEGATE1_CONST (QString, ViewToolPane, buttonText, ToolKey)

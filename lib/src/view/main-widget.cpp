#include <QButtonGroup>
#include <QRadioButton>
#include <QVBoxLayout>
#include "state.hpp"
#include "tools.hpp"
#include "view/gl-widget.hpp"
#include "view/main-widget.hpp"
#include "view/properties.hpp"
#include "view/util.hpp"

struct ViewMainWidget :: Impl {
  ViewMainWidget* self;
  ViewGlWidget    glWidget;
  ViewProperties  properties;
  QButtonGroup    toolButtons;

  Impl (ViewMainWidget* s, ViewMainWindow& mW, Config& config, Cache& cache) 
    : self       (s) 
    , glWidget   (mW, config, cache)
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
    leftPaneLayout->addWidget  (&ViewUtil::horizontalLine ());
    leftPaneLayout->addWidget  (&this->properties);
    leftPaneLayout->addStretch (1);

    return leftPane;
  }

  QWidget* initalizeToolPane () {
    QWidget*     toolPane       = new QWidget;
    QVBoxLayout* toolPaneLayout = new QVBoxLayout;

    toolPane->setLayout (toolPaneLayout);
    this->addToolButton <ToolMoveMesh>     (toolPaneLayout, QObject::tr ("Move mesh"));
    this->addToolButton <ToolDeleteMesh>   (toolPaneLayout, QObject::tr ("Delete mesh"));
    this->addToolButton <ToolNewMesh>      (toolPaneLayout, QObject::tr ("New mesh"));
    this->addToolButton <ToolSculptCarve>  (toolPaneLayout, QObject::tr ("Carve"));
    this->addToolButton <ToolSculptGrab>   (toolPaneLayout, QObject::tr ("Grab"));
    this->addToolButton <ToolSculptDrag>   (toolPaneLayout, QObject::tr ("Drag"));
    this->addToolButton <ToolSculptSmooth> (toolPaneLayout, QObject::tr ("Smooth"));
    return toolPane;
  }

  template <typename T>
  void addToolButton (QLayout* layout, const QString& name) {
    QRadioButton& button = ViewUtil::radioButton (name);
    
    ViewUtil::connect (button, [this] (bool value) {
      if (value) {
        State& s = this->glWidget.state ();
        s.resetTool (false);
        s.setTool   (std::move (*new T (s)));
      }
    });
    this->toolButtons.addButton (&button);
    layout->addWidget           (&button);
  }

  void deselectTool () {
    QAbstractButton* selectedButton = this->toolButtons.checkedButton ();
    if (selectedButton && this->glWidget.state ().hasTool () == false) {
      this->toolButtons.setExclusive (false);
      selectedButton->setChecked     (false);
      this->toolButtons.setExclusive (true);
    }
  }
};

DELEGATE3_BIG2_SELF (ViewMainWidget, ViewMainWindow&, Config&, Cache&)
GETTER   (ViewGlWidget&  , ViewMainWidget, glWidget)
GETTER   (ViewProperties&, ViewMainWidget, properties)
DELEGATE (void           , ViewMainWidget, deselectTool)

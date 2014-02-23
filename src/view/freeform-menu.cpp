#include "view/freeform-menu.hpp"
#include "state.hpp"
#include "tool/new-freeform-mesh.hpp"
#include "view/main-window.hpp"

struct ViewFreeformMenu::Impl {
  ViewFreeformMenu*  self;
  ViewMainWindow*    mainWindow;
  QContextMenuEvent* menuEvent;

  Impl (ViewFreeformMenu* s, ViewMainWindow* mW, QContextMenuEvent* e) 
    : self (s)
    , mainWindow (mW) 
    , menuEvent  (e) {

    this->addAction (tr("New Freeform Mesh"), new ToolNewFreeformMesh ());
  }

  void addAction (const QString& label, Tool* tool) {
    QAction* a = this->self->addAction (label);
    QObject::connect (a, &QAction::triggered, [this,tool] () { 
        State::setTool   (tool); 
        tool->initialize (this->mainWindow, this->menuEvent);
    });
  }
};

DELEGATE2_BIG3_SELF (ViewFreeformMenu, ViewMainWindow*, QContextMenuEvent*)

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

    this->addAction (new ToolNewFreeformMesh ());
  }

  void addAction (Tool* tool) {
    QAction* a = this->self->addAction (tool->toolName ());
    QObject::connect (a, &QAction::triggered, [this,tool] () { 
        State::setTool   (tool); 
        tool->initialize (this->mainWindow, this->menuEvent);
    });
  }
};

DELEGATE2_BIG3_SELF (ViewFreeformMenu, ViewMainWindow*, QContextMenuEvent*)

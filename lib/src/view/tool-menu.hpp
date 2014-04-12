#ifndef DILAY_VIEW_TOOL_MENU
#define DILAY_VIEW_TOOL_MENU

#include <QMenu>
#include "state.hpp"

class ViewMainWindow;
class QContextMenuEvent;

class ViewToolMenu : public QMenu {
  public:
    ViewToolMenu (ViewMainWindow* mW, QContextMenuEvent* mE)
      : mainWindow (mW)
      , menuEvent  (mE)
      {}

  template <typename T>
  void addAction () {
    QAction* a = this->QMenu::addAction (T::toolName ());
    QObject::connect (a, &QAction::triggered, [this] () { 
        State::setTool (new T (this->mainWindow, this->menuEvent));
    });
  }

  private:
    ViewMainWindow*    mainWindow;
    QContextMenuEvent* menuEvent;
};

#endif

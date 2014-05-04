#ifndef DILAY_VIEW_TOOL_MENU
#define DILAY_VIEW_TOOL_MENU

#include <QMenu>
#include <QString>
#include "state.hpp"

class ViewMainWindow;
class QContextMenuEvent;

class ViewToolMenu : public QMenu {
  public:
    ViewToolMenu (ViewMainWindow* mW, QContextMenuEvent* mE)
      : mainWindow (mW)
      , menuEvent  (mE)
      {}

  template <typename T, typename ... Args>
  void addAction (const Args& ... args) {
    QAction* a = this->QMenu::addAction (T::toolName (args ...));
    QObject::connect (a, &QAction::triggered, [this, args ...] () { 
        State::setTool (new T (this->mainWindow, this->menuEvent, args ...));
    });
  }

  private:
    ViewMainWindow*    mainWindow;
    QContextMenuEvent* menuEvent;
};

#endif

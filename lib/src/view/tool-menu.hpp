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

  template <typename T>
  void addAction () {
    QAction* a = this->QMenu::addAction (T::toolName ());
    QObject::connect (a, &QAction::triggered, [this] () { 
        State::setTool (new T (this->mainWindow, this->menuEvent));
    });
  }

  // TODO: replace by variadic template
  template <typename T, typename A1>
  void addAction (const A1& a1) {
    QAction* a = this->QMenu::addAction (T::toolName (a1));
    QObject::connect (a, &QAction::triggered, [this, a1] () { 
        State::setTool (new T (this->mainWindow, this->menuEvent, a1));
    });
  }

  private:
    ViewMainWindow*    mainWindow;
    QContextMenuEvent* menuEvent;
};

#endif

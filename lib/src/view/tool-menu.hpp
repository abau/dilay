#ifndef DILAY_VIEW_TOOL_MENU
#define DILAY_VIEW_TOOL_MENU

#include <glm/glm.hpp>
#include <QMenu>
#include <QString>
#include "state.hpp"

class ViewMainWindow;
class QContextMenuEvent;

class ViewToolMenu : public QMenu {
  public:
    ViewToolMenu (ViewMainWindow& mW, const glm::ivec2& p)
      : mainWindow   (mW)
      , menuPosition (p)
      {}

  template <typename T, typename ... Args>
  void addAction (const Args& ... args) {
    QAction* a = this->QMenu::addAction (T::toolName (args ...));
    QObject::connect (a, &QAction::triggered, [this, args ...] () { 
        State::setTool (new T (this->mainWindow, this->menuPosition, args ...));
    });
  }

  private:
    ViewMainWindow&  mainWindow;
    const glm::ivec2 menuPosition;
};

#endif

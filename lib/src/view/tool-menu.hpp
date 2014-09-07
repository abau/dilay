#ifndef DILAY_VIEW_TOOL_MENU
#define DILAY_VIEW_TOOL_MENU

#include <QMenu>
#include <glm/fwd.hpp>
#include "state.hpp"
#include "view/tool-menu-parameters.hpp"

class ViewMainWindow;
class ViewToolMenuParameters;

class ViewToolMenu : public QMenu {
  public:
    DECLARE_BIG3 (ViewToolMenu, ViewMainWindow&, const glm::ivec2&)

    ViewToolMenuParameters menuParameters () const;

    template <typename T, typename ... Args>
    void addAction (const Args& ... args) {
      QAction* a = this->QMenu::addAction (T::toolName (args ...));
      QObject::connect (a, &QAction::triggered, [this, args ...] () { 
          State::setTool (new T ( this->menuParameters ()
                                , args ...));
      });
    }

  protected:
    void mouseReleaseEvent (QMouseEvent*);

  private:
    IMPLEMENTATION
};

#endif

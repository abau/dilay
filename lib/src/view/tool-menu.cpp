#include <glm/glm.hpp>
#include <QMouseEvent>
#include "view/tool-menu.hpp"
#include "view/tool-menu-parameters.hpp"

struct ViewToolMenu::Impl {
  ViewToolMenu*    self;
  ViewMainWindow&  mainWindow;
  const glm::ivec2 menuPosition;
  bool             rightClick;

  Impl (ViewToolMenu* s, ViewMainWindow& mW, const glm::ivec2& p)
    : self         (s)
    , mainWindow   (mW)
    , menuPosition (p)
  {
    this->self->setContextMenuPolicy (Qt::PreventContextMenu); 
  }

  ViewToolMenuParameters menuParameters () const {
    return ViewToolMenuParameters ( this->mainWindow
                                  , this->menuPosition
                                  , this->rightClick);
  }

  void mouseReleaseEvent (QMouseEvent* e) {
    if (e->button () == Qt::LeftButton) {
      this->rightClick = false;
    }
    else if (e->button () == Qt::RightButton) {
      this->rightClick = true;
    }
    this->self->QMenu::mouseReleaseEvent (e);
  }
};

DELEGATE2_BIG3_SELF (ViewToolMenu, ViewMainWindow&, const glm::ivec2&)
DELEGATE_CONST (ViewToolMenuParameters, ViewToolMenu, menuParameters)
DELEGATE1      (void                  , ViewToolMenu, mouseReleaseEvent, QMouseEvent*)

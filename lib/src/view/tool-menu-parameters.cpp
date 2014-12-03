#include <glm/glm.hpp>
#include "view/tool-menu-parameters.hpp"

struct ViewToolMenuParameters::Impl {
        ViewMainWindow& mainWindow;
  const glm::ivec2      clickPosition;
  const bool            rightClick;

  Impl (ViewMainWindow& w, const glm::ivec2& p, bool r)
    : mainWindow    (w)
    , clickPosition (p)
    , rightClick    (r)
  {}
};

DELEGATE3_BIG4COPY (ViewToolMenuParameters, ViewMainWindow&, const glm::ivec2&, bool)
GETTER_CONST (ViewMainWindow&  , ViewToolMenuParameters, mainWindow)
GETTER_CONST (const glm::ivec2&, ViewToolMenuParameters, clickPosition)
GETTER_CONST (bool             , ViewToolMenuParameters, rightClick)

#include <glm/glm.hpp>
#include "view/tool/menu-parameters.hpp"

struct ViewToolMenuParameters::Impl {
        ViewMainWindow& mainWindow;
  const glm::ivec2      clickPosition;

  Impl (ViewMainWindow& w, const glm::ivec2& p)
    : mainWindow    (w)
    , clickPosition (p)
  {}
};

DELEGATE2_BIG4COPY (ViewToolMenuParameters, ViewMainWindow&, const glm::ivec2&)
GETTER_CONST (ViewMainWindow&  , ViewToolMenuParameters, mainWindow)
GETTER_CONST (const glm::ivec2&, ViewToolMenuParameters, clickPosition)

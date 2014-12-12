#include <glm/glm.hpp>
#include <QString>
#include "view/tool/menu-parameters.hpp"

struct ViewToolMenuParameters::Impl {
  const glm::ivec2 clickPosition;
  const QString    label;

  Impl (const glm::ivec2& p, const QString& l)
    : clickPosition (p)
    , label         (l)
  {}
};

DELEGATE2_BIG4COPY (ViewToolMenuParameters, const glm::ivec2&, const QString&)
GETTER_CONST   (const glm::ivec2&, ViewToolMenuParameters, clickPosition)
GETTER_CONST   (const QString&   , ViewToolMenuParameters, label)

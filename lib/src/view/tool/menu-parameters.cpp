#include <glm/glm.hpp>
#include <QString>
#include "view/tool/menu-parameters.hpp"

struct ViewToolMenuParameters::Impl {
        State&     state;
  const glm::ivec2 clickPosition;
  const QString    label;

  Impl (State& s, const glm::ivec2& p, const QString& l)
    : state         (s)
    , clickPosition (p)
    , label         (l)
  {}
};

DELEGATE3_BIG4COPY (ViewToolMenuParameters, State&, const glm::ivec2&, const QString&)
GETTER_CONST   (State&           , ViewToolMenuParameters, state)
GETTER_CONST   (const glm::ivec2&, ViewToolMenuParameters, clickPosition)
GETTER_CONST   (const QString&   , ViewToolMenuParameters, label)

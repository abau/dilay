#include <glm/glm.hpp>
#include <QString>
#include "maybe.hpp"
#include "view/tool/menu-parameters.hpp"

struct ViewToolMenuParameters::Impl {
  const glm::ivec2      clickPosition;
  const Maybe <QString> maybeLabel;

  Impl (const glm::ivec2& p)
    : clickPosition (p)
  {}

  Impl (const glm::ivec2& p, const QString& l)
    : clickPosition (p)
    , maybeLabel    (l)
  {}

  bool hasLabel () const {
    return this->maybeLabel.isSet ();
  }

  const QString& label () const {
    return this->maybeLabel.getRef ();
  }
};

DELEGATE1_BIG4COPY    (ViewToolMenuParameters, const glm::ivec2&)
DELEGATE2_CONSTRUCTOR (ViewToolMenuParameters, const glm::ivec2&, const QString&)
GETTER_CONST   (const glm::ivec2&, ViewToolMenuParameters, clickPosition)
DELEGATE_CONST (bool             , ViewToolMenuParameters, hasLabel)
DELEGATE_CONST (const QString&   , ViewToolMenuParameters, label)

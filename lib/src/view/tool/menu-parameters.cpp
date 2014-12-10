#include <glm/glm.hpp>
#include <QString>
#include "maybe.hpp"
#include "view/tool/menu-parameters.hpp"

struct ViewToolMenuParameters::Impl {
        ViewMainWindow& mainWindow;
  const glm::ivec2      clickPosition;
  const Maybe <QString> maybeLabel;

  Impl (ViewMainWindow& w, const glm::ivec2& p)
    : mainWindow    (w)
    , clickPosition (p)
  {}

  Impl (ViewMainWindow& w, const glm::ivec2& p, const QString& l)
    : mainWindow    (w)
    , clickPosition (p)
    , maybeLabel    (l)
  {}

  bool hasLabel () const {
    return this->maybeLabel.isSet ();
  }

  const QString& label () const {
    return this->maybeLabel.getRef ();
  }
};

DELEGATE2_BIG4COPY    (ViewToolMenuParameters, ViewMainWindow&, const glm::ivec2&)
DELEGATE3_CONSTRUCTOR (ViewToolMenuParameters, ViewMainWindow&, const glm::ivec2&, const QString&)
GETTER_CONST   (ViewMainWindow&  , ViewToolMenuParameters, mainWindow)
GETTER_CONST   (const glm::ivec2&, ViewToolMenuParameters, clickPosition)
DELEGATE_CONST (bool             , ViewToolMenuParameters, hasLabel)
DELEGATE_CONST (const QString&   , ViewToolMenuParameters, label)

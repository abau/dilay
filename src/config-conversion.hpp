#ifndef DILAY_CONFIG_CONVERSION
#define DILAY_CONFIG_CONVERSION

#include <glm/fwd.hpp>

class QDomElement;
class Color;

namespace ConfigConversion {
  bool fromDomElement (QDomElement&, float&);
  bool fromDomElement (QDomElement&, int&);
  bool fromDomElement (QDomElement&, glm::vec3&);
  bool fromDomElement (QDomElement&, Color&);
}

#endif

#ifndef DILAY_CONFIG_CONVERSION
#define DILAY_CONFIG_CONVERSION

#include <glm/fwd.hpp>
#include <string>

class QDomDocument;
class QDomElement;
class Color;

namespace ConfigConversion {
  bool fromDomElement (QDomElement, float&);
  bool fromDomElement (QDomElement, int&);
  bool fromDomElement (QDomElement, glm::vec3&);
  bool fromDomElement (QDomElement, Color&);

  QDomElement toDomElement (QDomDocument, const std::string&, const float&);
  QDomElement toDomElement (QDomDocument, const std::string&, const int&);
  QDomElement toDomElement (QDomDocument, const std::string&, const glm::vec3&);
  QDomElement toDomElement (QDomDocument, const std::string&, const Color&);
}

#endif

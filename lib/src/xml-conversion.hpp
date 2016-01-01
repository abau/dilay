/* This file is part of Dilay
 * Copyright © 2015,2016 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_CONFIG_CONVERSION
#define DILAY_CONFIG_CONVERSION

#include <glm/fwd.hpp>

class QDomDocument;
class QDomElement;
class Color;

namespace XmlConversion {
  bool fromDomElement (QDomElement, float&);
  bool fromDomElement (QDomElement, int&);
  bool fromDomElement (QDomElement, bool&);
  bool fromDomElement (QDomElement, glm::vec3&);
  bool fromDomElement (QDomElement, glm::ivec2&);
  bool fromDomElement (QDomElement, Color&);

  QDomElement& toDomElement (QDomDocument&, QDomElement&, const float&);
  QDomElement& toDomElement (QDomDocument&, QDomElement&, const int&);
  QDomElement& toDomElement (QDomDocument&, QDomElement&, const bool&);
  QDomElement& toDomElement (QDomDocument&, QDomElement&, const glm::vec3&);
  QDomElement& toDomElement (QDomDocument&, QDomElement&, const glm::ivec2&);
  QDomElement& toDomElement (QDomDocument&, QDomElement&, const Color&);
}

#endif

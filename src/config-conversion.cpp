#include <glm/glm.hpp>
#include <QDomElement>
#include "config-conversion.hpp"
#include "color.hpp"

bool ConfigConversion :: fromDomElement (QDomElement& e, float& v) {
  assert (e.attributeNode ("type").value () == "float");
  bool ok = true;
  v = e.text ().toFloat (&ok);
  return ok;
}

bool ConfigConversion :: fromDomElement (QDomElement& e, int& v) {
  assert (e.attributeNode ("type").value () == "integer");
  bool ok = true;
  v = e.text ().toInt (&ok);
  return ok;
}

bool ConfigConversion :: fromDomElement (QDomElement& e, glm::vec3& v) {
  assert (e.attributeNode ("type").value () == "vector3");
  bool okX = true;
  v.x = e.firstChildElement ("x").text ().toFloat (&okX);
  bool okY = true;
  v.y = e.firstChildElement ("y").text ().toFloat (&okY);
  bool okZ = true;
  v.z = e.firstChildElement ("z").text ().toFloat (&okZ);
  return okX && okY && okZ;
}

bool ConfigConversion :: fromDomElement (QDomElement& e, Color& v) {
  assert (e.attributeNode ("type").value () == "color");
  bool okR = true;
  v.r (e.firstChildElement ("r").text ().toFloat (&okR));
  bool okG = true;
  v.g (e.firstChildElement ("g").text ().toFloat (&okG));
  bool okB = true;
  v.b (e.firstChildElement ("b").text ().toFloat (&okB));
  return okR && okG && okB;
}

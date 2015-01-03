#include <QDomElement>
#include <glm/glm.hpp>
#include "color.hpp"
#include "config-conversion.hpp"

bool ConfigConversion :: fromDomElement (QDomElement e, float& v) {
  assert (e.attributeNode ("type").value () == "float");
  bool ok = true;
  v = e.text ().toFloat (&ok);
  return ok;
}

bool ConfigConversion :: fromDomElement (QDomElement e, int& v) {
  assert (e.attributeNode ("type").value () == "integer");
  bool ok = true;
  v = e.text ().toInt (&ok);
  return ok;
}

bool ConfigConversion :: fromDomElement (QDomElement e, bool& v) {
  assert (e.attributeNode ("type").value () == "boolean");
  bool ok = true;
  v = e.text ().toInt (&ok) != 0;
  return ok;
}

bool ConfigConversion :: fromDomElement (QDomElement e, glm::vec3& v) {
  assert (e.attributeNode ("type").value () == "vector3f");

  bool okX = ConfigConversion::fromDomElement (e.firstChildElement ("x"), v.x);
  bool okY = ConfigConversion::fromDomElement (e.firstChildElement ("y"), v.y);
  bool okZ = ConfigConversion::fromDomElement (e.firstChildElement ("z"), v.z);

  return okX && okY && okZ;
}

bool ConfigConversion :: fromDomElement (QDomElement e, glm::ivec2& v) {
  assert (e.attributeNode ("type").value () == "vector2i");

  bool okX = ConfigConversion::fromDomElement (e.firstChildElement ("x"), v.x);
  bool okY = ConfigConversion::fromDomElement (e.firstChildElement ("y"), v.y);

  return okX && okY;
}

bool ConfigConversion :: fromDomElement (QDomElement e, Color& v) {
  assert (e.attributeNode ("type").value () == "color");

  float tmp;
  bool okR = ConfigConversion::fromDomElement (e.firstChildElement ("r"), tmp);
  v.r (tmp);
  bool okG = ConfigConversion::fromDomElement (e.firstChildElement ("g"), tmp);
  v.g (tmp);
  bool okB = ConfigConversion::fromDomElement (e.firstChildElement ("b"), tmp);
  v.b (tmp);
  return okR && okG && okB;
}

QDomElement& ConfigConversion :: toDomElement ( QDomDocument& doc, QDomElement& elem
                                              , const float& v) {
  elem.setAttribute ("type", "float");
  elem.appendChild  (doc.createTextNode (std::to_string (v).c_str ()));
  return elem;
}

QDomElement& ConfigConversion :: toDomElement ( QDomDocument& doc, QDomElement& elem
                                              , const int& v) {
  elem.setAttribute ("type", "integer");
  elem.appendChild  (doc.createTextNode (std::to_string (v).c_str ()));
  return elem;
}

QDomElement& ConfigConversion :: toDomElement ( QDomDocument& doc, QDomElement& elem
                                              , const bool& v) {
  elem.setAttribute ("type", "boolean");
  elem.appendChild  (doc.createTextNode (std::to_string (v ? 1 : 0).c_str ()));
  return elem;
}

QDomElement& ConfigConversion :: toDomElement ( QDomDocument& doc, QDomElement& elem
                                              , const glm::vec3& v) {
  elem.setAttribute ("type", "vector3f");
  QDomElement x = doc.createElement ("x");
  QDomElement y = doc.createElement ("y");
  QDomElement z = doc.createElement ("z");
  elem.appendChild (ConfigConversion::toDomElement (doc, x, v.x));
  elem.appendChild (ConfigConversion::toDomElement (doc, y, v.y));
  elem.appendChild (ConfigConversion::toDomElement (doc, z, v.z));
  return elem;
}

QDomElement& ConfigConversion :: toDomElement ( QDomDocument& doc, QDomElement& elem
                                              , const glm::ivec2& v) {
  elem.setAttribute ("type", "vector2i");
  QDomElement x = doc.createElement ("x");
  QDomElement y = doc.createElement ("y");
  elem.appendChild (ConfigConversion::toDomElement (doc, x, v.x));
  elem.appendChild (ConfigConversion::toDomElement (doc, y, v.y));
  return elem;
}

QDomElement& ConfigConversion :: toDomElement ( QDomDocument& doc, QDomElement& elem
                                              , const Color& v) {
  elem.setAttribute ("type", "color");
  QDomElement r = doc.createElement ("r");
  QDomElement g = doc.createElement ("g");
  QDomElement b = doc.createElement ("b");
  elem.appendChild (ConfigConversion::toDomElement (doc, r, v.r ()));
  elem.appendChild (ConfigConversion::toDomElement (doc, g, v.g ()));
  elem.appendChild (ConfigConversion::toDomElement (doc, b, v.b ()));
  return elem;
}

/* This file is part of Dilay
 * Copyright © 2015 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <QDomElement>
#include <glm/glm.hpp>
#include "color.hpp"
#include "xml-conversion.hpp"

bool XmlConversion :: fromDomElement (QDomElement e, float& v) {
  assert (e.attributeNode ("type").value () == "float");
  bool ok = true;
  v = e.text ().toFloat (&ok);
  return ok;
}

bool XmlConversion :: fromDomElement (QDomElement e, int& v) {
  assert (e.attributeNode ("type").value () == "integer");
  bool ok = true;
  v = e.text ().toInt (&ok);
  return ok;
}

bool XmlConversion :: fromDomElement (QDomElement e, bool& v) {
  assert (e.attributeNode ("type").value () == "boolean");
  bool ok = true;
  v = e.text ().toInt (&ok) != 0;
  return ok;
}

bool XmlConversion :: fromDomElement (QDomElement e, glm::vec3& v) {
  assert (e.attributeNode ("type").value () == "vector3f");

  bool okX = XmlConversion::fromDomElement (e.firstChildElement ("x"), v.x);
  bool okY = XmlConversion::fromDomElement (e.firstChildElement ("y"), v.y);
  bool okZ = XmlConversion::fromDomElement (e.firstChildElement ("z"), v.z);

  return okX && okY && okZ;
}

bool XmlConversion :: fromDomElement (QDomElement e, glm::ivec2& v) {
  assert (e.attributeNode ("type").value () == "vector2i");

  bool okX = XmlConversion::fromDomElement (e.firstChildElement ("x"), v.x);
  bool okY = XmlConversion::fromDomElement (e.firstChildElement ("y"), v.y);

  return okX && okY;
}

bool XmlConversion :: fromDomElement (QDomElement e, Color& v) {
  assert (e.attributeNode ("type").value () == "color");

  float tmp;
  bool okR = XmlConversion::fromDomElement (e.firstChildElement ("r"), tmp);
  v.r (tmp);
  bool okG = XmlConversion::fromDomElement (e.firstChildElement ("g"), tmp);
  v.g (tmp);
  bool okB = XmlConversion::fromDomElement (e.firstChildElement ("b"), tmp);
  v.b (tmp);
  return okR && okG && okB;
}

QDomElement& XmlConversion :: toDomElement ( QDomDocument& doc, QDomElement& elem
                                           , const float& v) {
  elem.setAttribute ("type", "float");
  elem.appendChild  (doc.createTextNode (std::to_string (v).c_str ()));
  return elem;
}

QDomElement& XmlConversion :: toDomElement ( QDomDocument& doc, QDomElement& elem
                                           , const int& v) {
  elem.setAttribute ("type", "integer");
  elem.appendChild  (doc.createTextNode (std::to_string (v).c_str ()));
  return elem;
}

QDomElement& XmlConversion :: toDomElement ( QDomDocument& doc, QDomElement& elem
                                           , const bool& v) {
  elem.setAttribute ("type", "boolean");
  elem.appendChild  (doc.createTextNode (std::to_string (v ? 1 : 0).c_str ()));
  return elem;
}

QDomElement& XmlConversion :: toDomElement ( QDomDocument& doc, QDomElement& elem
                                           , const glm::vec3& v) {
  elem.setAttribute ("type", "vector3f");
  QDomElement x = doc.createElement ("x");
  QDomElement y = doc.createElement ("y");
  QDomElement z = doc.createElement ("z");
  elem.appendChild (XmlConversion::toDomElement (doc, x, v.x));
  elem.appendChild (XmlConversion::toDomElement (doc, y, v.y));
  elem.appendChild (XmlConversion::toDomElement (doc, z, v.z));
  return elem;
}

QDomElement& XmlConversion :: toDomElement ( QDomDocument& doc, QDomElement& elem
                                           , const glm::ivec2& v) {
  elem.setAttribute ("type", "vector2i");
  QDomElement x = doc.createElement ("x");
  QDomElement y = doc.createElement ("y");
  elem.appendChild (XmlConversion::toDomElement (doc, x, v.x));
  elem.appendChild (XmlConversion::toDomElement (doc, y, v.y));
  return elem;
}

QDomElement& XmlConversion :: toDomElement ( QDomDocument& doc, QDomElement& elem
                                           , const Color& v) {
  elem.setAttribute ("type", "color");
  QDomElement r = doc.createElement ("r");
  QDomElement g = doc.createElement ("g");
  QDomElement b = doc.createElement ("b");
  elem.appendChild (XmlConversion::toDomElement (doc, r, v.r ()));
  elem.appendChild (XmlConversion::toDomElement (doc, g, v.g ()));
  elem.appendChild (XmlConversion::toDomElement (doc, b, v.b ()));
  return elem;
}

#include <glm/glm.hpp>
#include <QDomElement>
#include "config-conversion.hpp"
#include "color.hpp"

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

bool ConfigConversion :: fromDomElement (QDomElement e, glm::vec3& v) {
  assert (e.attributeNode ("type").value () == "vector3");

  bool okX = ConfigConversion::fromDomElement (e.firstChildElement ("x"), v.x);
  bool okY = ConfigConversion::fromDomElement (e.firstChildElement ("y"), v.y);
  bool okZ = ConfigConversion::fromDomElement (e.firstChildElement ("z"), v.z);

  return okX && okY && okZ;
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

QDomElement withType (QDomDocument doc, const std::string& name, const std::string& type) {
  QDomElement e = doc.createElement (name.c_str ());
  e.setAttribute ("type", type.c_str ());
  return e;
}

QDomElement withType ( QDomDocument doc, const std::string& name, const std::string& type
                     , QDomNode node) {
  QDomElement e = withType (doc, name,type);
  e.appendChild  (node);
  return e;
}

QDomElement withType ( QDomDocument doc, const std::string& name, const std::string& type
                     , const std::string& v) {
  QDomText text = doc.createTextNode (v.c_str ());
  return withType (doc, name, type, text);
}

QDomElement ConfigConversion :: toDomElement ( QDomDocument doc, const std::string& name
                                             , const float& v) {
  return withType (doc, name, "float", std::to_string (v));
}

QDomElement ConfigConversion :: toDomElement ( QDomDocument doc, const std::string& name
                                             , const int& v) {
  return withType (doc, name, "integer", std::to_string (v));
}

QDomElement ConfigConversion :: toDomElement ( QDomDocument doc, const std::string& name
                                             , const glm::vec3& v) {
  QDomElement e = withType (doc, name, "vector3");
  e.appendChild (ConfigConversion::toDomElement (doc, "x", v.x));
  e.appendChild (ConfigConversion::toDomElement (doc, "y", v.y));
  e.appendChild (ConfigConversion::toDomElement (doc, "z", v.z));
  return e;
}

QDomElement ConfigConversion :: toDomElement ( QDomDocument doc, const std::string& name
                                             , const Color& v) {
  QDomElement e = withType (doc, name, "color");
  e.appendChild (ConfigConversion::toDomElement (doc, "r", v.r ()));
  e.appendChild (ConfigConversion::toDomElement (doc, "g", v.g ()));
  e.appendChild (ConfigConversion::toDomElement (doc, "b", v.b ()));
  return e;
}

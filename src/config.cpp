#include <glm/glm.hpp>
#include <unordered_map>
#include <algorithm>
#include <QDomNode>
#include <QFile>
#include <QTextStream>
#include "config.hpp"
#include "macro.hpp"
#include "variant.hpp"
#include "color.hpp"
#include "config-conversion.hpp"

typedef Variant <float,int,glm::vec3,glm::ivec2,Color> Value;

typedef std::unordered_map <std::string, Value> ConfigMap;

struct Config::Impl {
  const std::string optionsFileName;
  const std::string cacheFileName;
  const std::string cacheRoot;

  ConfigMap options;
  ConfigMap cache;

  Impl (const std::string& o, const std::string& c) 
    : optionsFileName (o) 
    , cacheFileName   (c) 
    , cacheRoot       ("cache")
  {
    this->loadFile (o, this->options, false);
    this->loadFile (c, this->cache  , true);
  }

  template <class T>
  const T& get (const std::string& path) const {
    ConfigMap::const_iterator value = this->options.find (path);

    if (value == this->options.end ()) {
      throw (std::runtime_error ("Can not find config path " + path));
    }
    return value->second.get <T> ();
  }

  template <class T>
  const T& get (const std::string& path, const T& defaultV) const {
    assert (path.find ("/" + this->cacheRoot) == 0);
    ConfigMap::const_iterator value = this->cache.find (path);

    if (value == this->cache.end ()) {
      return defaultV;
    }
    return value->second.get <T> ();
  }

  template <class T>
  void set (const std::string& path, const T& t) {
    assert (path.find ("/" + this->cacheRoot) == 0);
    Value value;
    value.set <T>       (t);
    this->cache.erase   (path);
    this->cache.emplace (path, value);
  }

  void loadFile (const std::string& fileName, ConfigMap& configMap, bool allowEmpty) {
    QFile file (fileName.c_str ());
    if (file.open (QIODevice::ReadOnly) == false) {
      if (allowEmpty) {
        return;
      }
      else {
        throw (std::runtime_error ("Can not open configuration file '" + fileName + "'"));
      }
    }
    QDomDocument doc (fileName.c_str ());
    QString      errorMsg;
    int          errorLine   = -1;
    int          errorColumn = -1;
    if (doc.setContent (&file, &errorMsg, &errorLine, &errorColumn) == false) {
      file.close();
      throw (std::runtime_error 
          ( "Error while loading configuration file '" + fileName + "': " + errorMsg.toStdString ()
          + " (" + std::to_string (errorLine) + ","  + std::to_string (errorColumn) + ")"));
    }
    file.close();
    try {
      this->loadConfigMap (configMap, "", doc);
    }
    catch (std::runtime_error& e) {
      throw (std::runtime_error 
          ("Error while parsing configuration file '" + fileName + "': " + e.what ()));
    }
  }

  void loadConfigMap (ConfigMap& configMap, const QString& prefix, QDomNode& node) {
    QDomNode child = node.firstChild ();

    while (child.isNull () == false) {
      if (child.isElement()) {
          QDomElement element   = child.toElement();
          QDomAttr    attribute = element.attributeNode ("type");
          Value       value;

          if (attribute.isNull ()) {
            this->loadConfigMap (configMap, prefix + "/" + element.tagName (), child);
          }
          else if (attribute.value () == "float") {
            this->insertIntoConfigMap <float> (configMap, prefix, element);
          }
          else if (attribute.value () == "integer") {
            this->insertIntoConfigMap <int> (configMap, prefix, element);
          }
          else if (attribute.value () == "vector3f") {
            this->insertIntoConfigMap <glm::vec3> (configMap, prefix, element);
          }
          else if (attribute.value () == "vector2i") {
            this->insertIntoConfigMap <glm::ivec2> (configMap, prefix, element);
          }
          else if (attribute.value () == "color") {
            this->insertIntoConfigMap <Color> (configMap, prefix, element);
          }
          else {
            throw (std::runtime_error 
              ( "invalid type '" + attribute.value ().toStdString () + "' "
              + "(" + std::to_string (child.lineNumber   ()) 
              + "," + std::to_string (child.columnNumber ()) + ")"));
          }
      }
      child = child.nextSibling();
    }
  }

  template <typename T>
  bool insertIntoConfigMap (ConfigMap& configMap, const QString& prefix, QDomElement& element) {
    T           t;
    bool        ok  = ConfigConversion::fromDomElement (element, t);
    std::string key = (prefix + "/" + element.tagName ()).toStdString ();

    if (ok) {
      Value value;
      value.set <T> (t);
      configMap.emplace (key, value);
    }
    else {
      throw (std::runtime_error 
          ( "can not parse value of key '" + key + "' "
          + "(" + std::to_string (element.lineNumber   ()) 
          + "," + std::to_string (element.columnNumber ()) + ")"));
    }
    return ok;
  }

  void writeCache () {
    QDomDocument doc;

    for (auto& c : this->cache) {
      const std::string& key   = c.first;
            Value&       value = c.second;
            QStringList  path  = QString (key.c_str ()).split ("/", QString::SkipEmptyParts);

      this->appendAsDomChild (doc, doc, path, value);
    }
    if (doc.isNull () == false) {
      QFile file (this->cacheFileName.c_str ());
      if (file.open (QIODevice::WriteOnly)) {
        QTextStream stream (&file);
        doc.save (stream, 2);
        file.close ();
      }
    }
  }

  void appendAsDomChild (QDomDocument& doc, QDomNode& parent, QStringList& path, Value& value) {
    if (path.empty ()) {
      assert (parent.isElement ());
      QDomElement elem = parent.toElement ();
      if (value.is <float> ()) {
        ConfigConversion::toDomElement (doc, elem, value.get <float> ());
      }
      else if (value.is <int> ()) {
        ConfigConversion::toDomElement (doc, elem, value.get <int> ());
      }
      else if (value.is <glm::vec3> ()) {
        ConfigConversion::toDomElement (doc, elem, value.get <glm::vec3> ());
      }
      else if (value.is <glm::ivec2> ()) {
        ConfigConversion::toDomElement (doc, elem, value.get <glm::ivec2> ());
      }
      else if (value.is <Color> ()) {
        ConfigConversion::toDomElement (doc, elem, value.get <Color> ());
      }
      else
        assert (false);
    }
    else {
      const QString& head  = path.first ();
      QDomElement    child = parent.firstChildElement (head);
      if (child.isNull ()) {
        child = doc.createElement (head);
        if (parent.isNull ()) {
          doc.appendChild (child);
        }
        else {
          parent.appendChild (child);
        }
      }
      assert (child.hasAttribute ("type") == false);

      path.removeFirst ();
      this->appendAsDomChild (doc, child, path, value);
    }
  }
};

Config :: Config () : impl (new Impl ("dilay.options", "dilay.cache")) {}
DELEGATE_BIG3_WITHOUT_CONSTRUCTOR (Config)
GLOBAL (Config);

DELEGATE_GLOBAL (void, Config, writeCache);

template <class T>
const T& Config :: get (const std::string& path) {
  return Config::global ().impl->get<T> (path);
}

template <class T>
const T& Config :: get (const std::string& path, const T& defaultV) {
  return Config::global ().impl->get<T> (path, defaultV);
}

template <class T>
void Config :: set (const std::string& path, const T& value) {
  return Config::global ().impl->set<T> (path, value);
}

template const float&      Config :: get<float>      (const std::string&);
template const float&      Config :: get<float>      (const std::string&, const float&);
template void              Config :: set<float>      (const std::string&, const float&);
template const int&        Config :: get<int>        (const std::string&);
template const int&        Config :: get<int>        (const std::string&, const int&);
template void              Config :: set<int>        (const std::string&, const int&);
template const Color&      Config :: get<Color>      (const std::string&);
template const Color&      Config :: get<Color>      (const std::string&, const Color&);
template void              Config :: set<Color>      (const std::string&, const Color&);
template const glm::vec3&  Config :: get<glm::vec3>  (const std::string&);
template const glm::vec3&  Config :: get<glm::vec3>  (const std::string&, const glm::vec3&);
template void              Config :: set<glm::vec3>  (const std::string&, const glm::vec3&);
template const glm::ivec2& Config :: get<glm::ivec2> (const std::string&);
template const glm::ivec2& Config :: get<glm::ivec2> (const std::string&, const glm::ivec2&);
template void              Config :: set<glm::ivec2> (const std::string&, const glm::ivec2&);

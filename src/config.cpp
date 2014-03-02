#include <glm/glm.hpp>
#include <unordered_map>
#include <QDomNode>
#include <QFile>
#include "config.hpp"
#include "macro.hpp"
#include "variant.hpp"
#include "color.hpp"
#include "config-conversion.hpp"

typedef Variant <float,int,glm::vec3,Color> Value;

typedef std::unordered_map <std::string, Value> ConfigMap;

struct Config::Impl {
  const std::string fileName;

  ConfigMap configMap;

  Impl (const std::string& fn) : fileName (fn) {
    this->loadFile ();
  }

  template <class T>
  const T& get (const std::string& path) const {
    ConfigMap::const_iterator value = this->configMap.find (path);

    if (value == this->configMap.end ()) {
      throw (std::runtime_error ("Can not find config path " + path));
    }
    return *value->second.get <T> ();
  }

  void loadFile () {
    QFile file (this->fileName.c_str ());
    if (file.open (QIODevice::ReadOnly) == false) {
      throw (std::runtime_error ("Configuration file '" + this->fileName + "' not found"));
    }
    QDomDocument doc (this->fileName.c_str ());
    QString      errorMsg;
    int          errorLine   = -1;
    int          errorColumn = -1;
    if (doc.setContent (&file, &errorMsg, &errorLine, &errorColumn) == false) {
      file.close();
      this->throwError (errorMsg.toStdString (), errorLine, errorColumn);
    }
    file.close();
    this->loadConfigMap ("", doc);
  }

  void loadConfigMap (const QString& prefix, QDomNode& node) {
    QDomNode child = node.firstChild ();

    while (child.isNull () == false) {
      if (child.isElement()) {
          QDomElement element   = child.toElement();
          QDomAttr    attribute = element.attributeNode ("type");
          Value       value;

          if (attribute.isNull ()) {
            this->loadConfigMap (prefix + "/" + element.tagName (), child);
          }
          else if (attribute.value () == "float") {
            this->insertIntoConfigMap <float> (prefix,element);
          }
          else if (attribute.value () == "integer") {
            this->insertIntoConfigMap <int> (prefix,element);
          }
          else if (attribute.value () == "vector3") {
            this->insertIntoConfigMap <glm::vec3> (prefix,element);
          }
          else if (attribute.value () == "color") {
            this->insertIntoConfigMap <Color> (prefix,element);
          }
          else {
            this->throwError ("invalid type '" + attribute.value ().toStdString () + "'"
                             , child.lineNumber (), child.columnNumber ());
          }
      }
      child = child.nextSibling();
    }
  }

  template <typename T>
  bool insertIntoConfigMap (const QString& prefix, QDomElement& element) {
    T           t;
    bool        ok  = ConfigConversion::fromDomElement (element, t);
    std::string key = (prefix + "/" + element.tagName ()).toStdString ();

    if (ok) {
      Value value;
      value.set <T> (t);
      this->configMap.emplace (key, value);
    }
    else {
      this->throwError ("can't parse '" + key + "'", element.lineNumber (), element.columnNumber ());
    }
    return ok;
  }

  void throwError (const std::string& msg, int line, int column) {
    throw (std::runtime_error 
        ( "Error while loading configuration file '" + this->fileName + "': "
        + msg + " (" + std::to_string (line) + "," + std::to_string (column) + ")"));
  }
};

Config :: Config () : impl (new Impl ("dilay.config")) {}
DELEGATE_BIG3_WITHOUT_CONSTRUCTOR (Config)
GLOBAL (Config);

template <class T>
const T& Config :: get (const std::string& path) {
  return Config::global ().impl->get<T> (path);
}

template const float&     Config :: get<float>     (const std::string&);
template const int&       Config :: get<int>       (const std::string&);
template const Color&     Config :: get<Color>     (const std::string&);
template const glm::vec3& Config :: get<glm::vec3> (const std::string&);

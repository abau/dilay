#include <QDomNode>
#include <QFile>
#include <QTextStream>
#include <glm/glm.hpp>
#include <unordered_map>
#include "color.hpp"
#include "kvstore.hpp"
#include "util.hpp"
#include "variant.hpp"
#include "xml-conversion.hpp"

struct KVStore::Impl {
  typedef Variant <float,int,bool,glm::vec3,glm::ivec2,Color> Value;
  typedef std::unordered_map <std::string, Value>             Map;

  const std::string fileName;
  const std::string root;
        Map         map;

  Impl (const std::string& f, bool mustExist, const std::string& r) 
    : fileName (f)
    , root     (r)
  {
    assert (this->root.find ('/') == std::string::npos);

    this->loadFile (mustExist);
  }

  std::string path (const std::string& suffix) const {
    assert (suffix.front () != '/');
    assert (suffix.back  () != '/');

    return "/" + this->root + "/" + suffix;
  }

  template <class T>
  const T& get (const std::string& p) const {

    Map::const_iterator value = this->map.find (this->path (p));

    if (value == this->map.end ()) {
      throw (std::runtime_error ("Can not find path '" + this->path (p) + "' in kv-store"));
    }
    else {
      return value->second.get <T> ();
    }
  }

  template <class T>
  const T& get (const std::string& p, const T& defaultV) const {
    Map::const_iterator value = this->map.find (this->path (p));

    if (value == this->map.end ()) {
      return defaultV;
    }
    else {
      return value->second.get <T> ();
    }
  }

  template <class T>
  void set (const std::string& p, const T& t) {
    Value value;
    value.set <T>     (t);
    this->map.erase   (this->path (p));
    this->map.emplace (this->path (p), value);
  }

  void loadFile (bool mustExist) {
    QFile file (this->fileName.c_str ());

    if (file.open (QIODevice::ReadOnly) == false) {
      if (mustExist) {
        throw (std::runtime_error ("Can not open kv-store file '" + this->fileName + "'"));
      }
      else {
        return;
      }
    }
    QDomDocument doc (this->fileName.c_str ());
    QString      errorMsg;
    int          errorLine   = -1;
    int          errorColumn = -1;
    if (doc.setContent (&file, &errorMsg, &errorLine, &errorColumn) == false) {
      file.close();
      throw (std::runtime_error 
          ( "Error while loading kv-store file '" + this->fileName + "': " + errorMsg.toStdString ()
          + " (" + std::to_string (errorLine) + ","  + std::to_string (errorColumn) + ")"));
    }
    file.close();
    try {
      this->loadNode ("", doc);
    }
    catch (std::runtime_error& e) {
      throw (std::runtime_error 
          ("Error while parsing kv-store file '" + this->fileName + "': " + e.what ()));
    }
  }

  void loadNode (const QString& prefix, QDomNode& node) {
    QDomNode child = node.firstChild ();

    while (child.isNull () == false) {
      if (child.isElement()) {
          QDomElement element   = child.toElement();
          QDomAttr    attribute = element.attributeNode ("type");
          Value       value;

          if (attribute.isNull ()) {
            this->loadNode (prefix + "/" + element.tagName (), child);
          }
          else if (attribute.value () == "float") {
            this->insertIntoMap <float> (prefix, element);
          }
          else if (attribute.value () == "integer") {
            this->insertIntoMap <int> (prefix, element);
          }
          else if (attribute.value () == "boolean") {
            this->insertIntoMap <bool> (prefix, element);
          }
          else if (attribute.value () == "vector3f") {
            this->insertIntoMap <glm::vec3> (prefix, element);
          }
          else if (attribute.value () == "vector2i") {
            this->insertIntoMap <glm::ivec2> (prefix, element);
          }
          else if (attribute.value () == "color") {
            this->insertIntoMap <Color> (prefix, element);
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
  bool insertIntoMap (const QString& prefix, QDomElement& element) {
    T           t;
    bool        ok  = XmlConversion::fromDomElement (element, t);
    std::string key = (prefix + "/" + element.tagName ()).toStdString ();

    if (ok) {
      Value value;
      value.set <T> (t);
      this->map.emplace (key, value);
    }
    else {
      throw (std::runtime_error 
          ( "can not parse value of key '" + key + "' "
          + "(" + std::to_string (element.lineNumber   ()) 
          + "," + std::to_string (element.columnNumber ()) + ")"));
    }
    return ok;
  }

  void writeToDisk (bool mustSucceed) const {
    QDomDocument doc;

    for (auto& c : this->map) {
      const std::string& key   = c.first;
      const Value&       value = c.second;
            QStringList  path  = QString (key.c_str ()).split ("/", QString::SkipEmptyParts);

      this->appendAsDomChild (doc, doc, path, value);
    }
    if (doc.isNull () == false) {
      QFile file (this->fileName.c_str ());
      if (file.open (QIODevice::WriteOnly)) {
        QTextStream stream (&file);
        doc.save (stream, 2);
        file.close ();
      }
      else if (mustSucceed) {
        throw (std::runtime_error ("Can not save kv-store file '" + this->fileName + "'"));
      }
    }
  }

  void appendAsDomChild ( QDomDocument& doc, QDomNode& parent
                        , QStringList& path, const Value& value ) const
  {
    if (path.empty ()) {
      assert (parent.isElement ());
      QDomElement elem = parent.toElement ();
      if (value.is <float> ()) {
        XmlConversion::toDomElement (doc, elem, value.get <float> ());
      }
      else if (value.is <int> ()) {
        XmlConversion::toDomElement (doc, elem, value.get <int> ());
      }
      else if (value.is <bool> ()) {
        XmlConversion::toDomElement (doc, elem, value.get <bool> ());
      }
      else if (value.is <glm::vec3> ()) {
        XmlConversion::toDomElement (doc, elem, value.get <glm::vec3> ());
      }
      else if (value.is <glm::ivec2> ()) {
        XmlConversion::toDomElement (doc, elem, value.get <glm::ivec2> ());
      }
      else if (value.is <Color> ()) {
        XmlConversion::toDomElement (doc, elem, value.get <Color> ());
      }
      else
        DILAY_IMPOSSIBLE
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

DELEGATE3_BIG2  (KVStore, const std::string&, bool, const std::string&)
DELEGATE1_CONST (void, KVStore, writeToDisk, bool);

template <class T>
const T& KVStore :: get (const std::string& path) const {
  return this->impl->get<T> (path);
}

template <class T>
const T& KVStore :: get (const std::string& path, const T& defaultV) const {
  return this->impl->get<T> (path, defaultV);
}

template <class T>
void KVStore :: set (const std::string& path, const T& value) {
  return this->impl->set<T> (path, value);
}

template const float&      KVStore :: get<float>      (const std::string&) const;
template const float&      KVStore :: get<float>      (const std::string&, const float&) const;
template void              KVStore :: set<float>      (const std::string&, const float&);
template const int&        KVStore :: get<int>        (const std::string&) const;
template const int&        KVStore :: get<int>        (const std::string&, const int&) const;
template void              KVStore :: set<int>        (const std::string&, const int&);
template const bool&       KVStore :: get<bool>       (const std::string&) const;
template const bool&       KVStore :: get<bool>       (const std::string&, const bool&) const;
template void              KVStore :: set<bool>       (const std::string&, const bool&);
template const Color&      KVStore :: get<Color>      (const std::string&) const;
template const Color&      KVStore :: get<Color>      (const std::string&, const Color&) const;
template void              KVStore :: set<Color>      (const std::string&, const Color&);
template const glm::vec3&  KVStore :: get<glm::vec3>  (const std::string&) const;
template const glm::vec3&  KVStore :: get<glm::vec3>  (const std::string&, const glm::vec3&) const;
template void              KVStore :: set<glm::vec3>  (const std::string&, const glm::vec3&);
template const glm::ivec2& KVStore :: get<glm::ivec2> (const std::string&) const;
template const glm::ivec2& KVStore :: get<glm::ivec2> (const std::string&, const glm::ivec2&) const;
template void              KVStore :: set<glm::ivec2> (const std::string&, const glm::ivec2&);

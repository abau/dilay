/* This file is part of Dilay
 * Copyright © 2015-2017 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
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

struct KVStore::Impl
{
  typedef Variant<float, int, bool, glm::vec3, glm::ivec2, Color> Value;
  typedef std::unordered_map<std::string, Value> Map;

  const std::string root;
  Map               map;

  Impl (const std::string& r)
    : root (r)
  {
    assert (this->root.find ('/') == std::string::npos);
  }

  std::string path (const std::string& suffix) const
  {
    assert (suffix.back () != '/');

    if (suffix.front () == '/')
    {
      assert (suffix.find ("/" + this->root + "/") == 0);
      return suffix;
    }
    else
    {
      return "/" + this->root + "/" + suffix;
    }
  }

  template <class T> const T& get (const std::string& p) const
  {
    Map::const_iterator value = this->map.find (this->path (p));

    if (value == this->map.end ())
    {
      throw (std::runtime_error ("Can not find path '" + this->path (p) + "' in kv-store"));
    }
    else
    {
      return value->second.get<T> ();
    }
  }

  template <class T> const T& get (const std::string& p, const T& defaultV) const
  {
    Map::const_iterator value = this->map.find (this->path (p));

    if (value == this->map.end ())
    {
      return defaultV;
    }
    else
    {
      return value->second.get<T> ();
    }
  }

  template <class T> void set (const std::string& p, const T& t)
  {
    this->remove (p);

    Value value;
    value.set<T> (t);
    this->map.emplace (this->path (p), value);
  }

  void fromFile (const std::string& fileName)
  {
    Util::withCLocale<void> ([this, &fileName]() {
      QFile file (fileName.c_str ());

      if (file.open (QIODevice::ReadOnly | QIODevice::Text) == false)
      {
        throw (std::runtime_error ("Can not open kv-store file '" + fileName + "'"));
      }
      QDomDocument doc (fileName.c_str ());
      QString      errorMsg;
      int          errorLine = -1;
      int          errorColumn = -1;
      if (doc.setContent (&file, &errorMsg, &errorLine, &errorColumn) == false)
      {
        file.close ();
        throw (std::runtime_error (
          "Error while loading kv-store file '" + fileName + "': " + errorMsg.toStdString () +
          " (" + std::to_string (errorLine) + "," + std::to_string (errorColumn) + ")"));
      }
      file.close ();
      try
      {
        this->loadNode ("", doc);
      }
      catch (std::runtime_error& e)
      {
        throw (std::runtime_error ("Error while parsing kv-store file '" + fileName +
                                   "': " + e.what ()));
      }
    });
  }

  void loadNode (const QString& prefix, QDomNode& node)
  {
    QDomNode child = node.firstChild ();

    while (child.isNull () == false)
    {
      if (child.isElement ())
      {
        QDomElement element = child.toElement ();
        QDomAttr    attribute = element.attributeNode ("type");
        Value       value;

        if (attribute.isNull ())
        {
          this->loadNode (prefix + "/" + element.tagName (), child);
        }
        else if (attribute.value () == "float")
        {
          this->loadElement<float> (prefix, element);
        }
        else if (attribute.value () == "integer")
        {
          this->loadElement<int> (prefix, element);
        }
        else if (attribute.value () == "boolean")
        {
          this->loadElement<bool> (prefix, element);
        }
        else if (attribute.value () == "vector3f")
        {
          this->loadElement<glm::vec3> (prefix, element);
        }
        else if (attribute.value () == "vector2i")
        {
          this->loadElement<glm::ivec2> (prefix, element);
        }
        else if (attribute.value () == "color")
        {
          this->loadElement<Color> (prefix, element);
        }
        else
        {
          throw (std::runtime_error ("invalid type '" + attribute.value ().toStdString () + "' " +
                                     "(" + std::to_string (child.lineNumber ()) + "," +
                                     std::to_string (child.columnNumber ()) + ")"));
        }
      }
      else
      {
        throw (std::runtime_error ("invalid node (" + std::to_string (child.lineNumber ()) + "," +
                                   std::to_string (child.columnNumber ()) + ")"));
      }
      child = child.nextSibling ();
    }
  }

  template <typename T> bool loadElement (const QString& prefix, QDomElement& element)
  {
    T                 t;
    const bool        ok = XmlConversion::fromDomElement (element, t);
    const std::string suffix = element.tagName ().toStdString ();
    const std::string key = prefix.toStdString () + "/" + suffix;

    if (ok)
    {
      this->set<T> (key, t);
    }
    else
    {
      throw (std::runtime_error ("can not parse value of key '" + key + "' " + "(" +
                                 std::to_string (element.lineNumber ()) + "," +
                                 std::to_string (element.columnNumber ()) + ")"));
    }
    return ok;
  }

  void toFile (const std::string& fileName) const
  {
    Util::withCLocale<void> ([this, &fileName]() {
      QDomDocument doc;

      for (auto& c : this->map)
      {
        const std::string& key = c.first;
        const Value&       value = c.second;
        QStringList        path = QString (key.c_str ()).split ("/", QString::SkipEmptyParts);

        this->appendAsDomChild (doc, doc, path, value);
      }
      if (doc.isNull () == false)
      {
        QFile file (fileName.c_str ());
        if (file.open (QIODevice::WriteOnly | QIODevice::Text))
        {
          QTextStream stream (&file);
          doc.save (stream, 2);
          file.close ();
        }
        else
        {
          throw (std::runtime_error ("Can not save kv-store file '" + fileName + "'"));
        }
      }
    });
  }

  void appendAsDomChild (QDomDocument& doc, QDomNode& parent, QStringList& path,
                         const Value& value) const
  {
    if (path.empty ())
    {
      assert (parent.isElement ());
      QDomElement elem = parent.toElement ();
      if (value.is<float> ())
      {
        XmlConversion::toDomElement (doc, elem, value.get<float> ());
      }
      else if (value.is<int> ())
      {
        XmlConversion::toDomElement (doc, elem, value.get<int> ());
      }
      else if (value.is<bool> ())
      {
        XmlConversion::toDomElement (doc, elem, value.get<bool> ());
      }
      else if (value.is<glm::vec3> ())
      {
        XmlConversion::toDomElement (doc, elem, value.get<glm::vec3> ());
      }
      else if (value.is<glm::ivec2> ())
      {
        XmlConversion::toDomElement (doc, elem, value.get<glm::ivec2> ());
      }
      else if (value.is<Color> ())
      {
        XmlConversion::toDomElement (doc, elem, value.get<Color> ());
      }
      else
        DILAY_IMPOSSIBLE
    }
    else
    {
      const QString& head = path.first ();
      QDomElement    child = parent.firstChildElement (head);
      if (child.isNull ())
      {
        child = doc.createElement (head);
        if (parent.isNull ())
        {
          doc.appendChild (child);
        }
        else
        {
          parent.appendChild (child);
        }
      }
      assert (child.hasAttribute ("type") == false);

      path.removeFirst ();
      this->appendAsDomChild (doc, child, path, value);
    }
  }

  void remove (const std::string& p) { this->map.erase (this->path (p)); }

  void reset () { this->map.clear (); }
};

DELEGATE1_BIG2 (KVStore, const std::string&)
DELEGATE1 (void, KVStore, fromFile, const std::string&);
DELEGATE1_CONST (void, KVStore, toFile, const std::string&);
DELEGATE1 (void, KVStore, remove, const std::string&);
DELEGATE (void, KVStore, reset);

template <class T> const T& KVStore::get (const std::string& path) const
{
  return this->impl->get<T> (path);
}

template <class T> const T& KVStore::get (const std::string& path, const T& defaultV) const
{
  return this->impl->get<T> (path, defaultV);
}

template <class T> void KVStore::set (const std::string& path, const T& value)
{
  return this->impl->set<T> (path, value);
}

template const float&      KVStore::get<float> (const std::string&) const;
template const float&      KVStore::get<float> (const std::string&, const float&) const;
template void              KVStore::set<float> (const std::string&, const float&);
template const int&        KVStore::get<int> (const std::string&) const;
template const int&        KVStore::get<int> (const std::string&, const int&) const;
template void              KVStore::set<int> (const std::string&, const int&);
template const bool&       KVStore::get<bool> (const std::string&) const;
template const bool&       KVStore::get<bool> (const std::string&, const bool&) const;
template void              KVStore::set<bool> (const std::string&, const bool&);
template const Color&      KVStore::get<Color> (const std::string&) const;
template const Color&      KVStore::get<Color> (const std::string&, const Color&) const;
template void              KVStore::set<Color> (const std::string&, const Color&);
template const glm::vec3&  KVStore::get<glm::vec3> (const std::string&) const;
template const glm::vec3&  KVStore::get<glm::vec3> (const std::string&, const glm::vec3&) const;
template void              KVStore::set<glm::vec3> (const std::string&, const glm::vec3&);
template const glm::ivec2& KVStore::get<glm::ivec2> (const std::string&) const;
template const glm::ivec2& KVStore::get<glm::ivec2> (const std::string&, const glm::ivec2&) const;
template void              KVStore::set<glm::ivec2> (const std::string&, const glm::ivec2&);

#include <yaml-cpp/yaml.h>
#include <glm/glm.hpp>
#include <unordered_map>
#include "config.hpp"
#include "macro.hpp"
#include "config-conversion.hpp"

typedef std::unordered_map <std::string, YAML::Node> ConfigMap;

struct Config::Impl {
  YAML::Node document;
  ConfigMap  configMap;

  Impl (const std::string& fileName) {
    try {
      this->document = YAML::LoadFile (fileName);
      this->loadConfigMap ("",this->document);
    }
    catch (YAML::Exception e) {
      throw (std::runtime_error ( "YAML::Exception when loading file " + fileName
                                + ": " + e.what ()));
    }
  }

  /** Builds a map from paths to yaml nodes where each path references a
   * scalar value. Sequences are inserted per element (with the element's index 
   * as last path segment) and as complete node.
   */
  void loadConfigMap (const std::string& prefix, const YAML::Node& node) {
    if (node.IsMap ()) {
      for (YAML::const_iterator it = node.begin (); it != node.end (); ++it)
        this->loadConfigMap (prefix + "/" + it->first.as <std::string>(), it->second);
    }
    else if (node.IsSequence ()) {
      this->configMap.insert (ConfigMap::value_type (prefix, node));

      unsigned int i = 0;
      for (YAML::const_iterator it = node.begin (); it != node.end (); ++it, ++i) {
        this->loadConfigMap (prefix + "/" + std::to_string (i), *it);
      }
    }
    else {
      this->configMap.insert (ConfigMap::value_type (prefix, node));
    }
  }

  template <class T>
  T get (const std::string& path) const {
    ConfigMap::const_iterator value = this->configMap.find (path);

    if (value == this->configMap.end ()) {
      throw (std::runtime_error ("Can not find config path " + path));
    }
    return value->second.as<T> ();
  }
};

Config :: Config () : impl (new Impl ("dilay.config")) {}

DELEGATE_DESTRUCTOR (Config)
GLOBAL              (Config);

template <class T>
T Config :: get (const std::string& path) {
  return Config::global ().impl->get<T> (path);
}

template float     Config :: get<float>     (const std::string&);
template int       Config :: get<int>       (const std::string&);
template Color     Config :: get<Color>     (const std::string&);
template glm::vec3 Config :: get<glm::vec3> (const std::string&);

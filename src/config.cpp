#include <yaml-cpp/yaml.h>
#include <unordered_map>
#include "config.hpp"
#include "macro.hpp"

typedef std::unordered_map <std::string, YAML::Node> ConfigMap;

struct ConfigImpl {
  YAML::Node document;
  ConfigMap  configMap;

  ConfigImpl (const std::string& fileName) {
    try {
      this->document = YAML::LoadFile (fileName);
      this->loadConfigMap ("",this->document);
    }
    catch (YAML::Exception e) {
      throw (std::runtime_error ( "YAML::Exception when loading file " + fileName
                                + ": " + e.what ()));
    }
  }

  void loadConfigMap (const std::string& prefix, const YAML::Node& node) {
    if (node.IsMap ()) {
      for (YAML::const_iterator it = node.begin (); it != node.end (); ++it)
        this->loadConfigMap (prefix + "/" + it->first.as <std::string>(), it->second);
    }
    else if (node.IsSequence ()) {
      unsigned int i = 0;
      for (YAML::const_iterator it = node.begin (); it != node.end (); ++it, ++i) {
        this->loadConfigMap (prefix + "/" + std::to_string (i), *it);
      }
    }
    else {
      ConfigMap::value_type value (prefix, node);
      this->configMap.insert (value);
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

Config :: Config () : impl (new ConfigImpl ("dilay.config")) {}

DELEGATE_DESTRUCTOR (Config)
GLOBAL              (Config);

template <class T>
T Config :: get (const std::string& path) const {
  return this->impl->get<T> (path);
}

template float Config :: get<float> (const std::string&) const;
template int   Config :: get<int>   (const std::string&) const;

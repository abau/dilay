#include <yaml-cpp/yaml.h>
#include "config.hpp"
#include "util.hpp"
#include "macro.hpp"

struct ConfigImpl {
  YAML::Node document;

  ConfigImpl (const std::string& fileName) {
    try {
      this->document = YAML::LoadFile (fileName);
    }
    catch (YAML::Exception e) {
      throw (std::runtime_error ( "YAML::Exception when loading file " + fileName
                                + ": " + e.what ()));
    }
  }

  template <class T>
  T get (const std::initializer_list <std::string>& path) const {
    try {
      YAML::Node node = YAML::Clone (this->document);
      for (const std::string& p : path) {
        node = node[p];
      }
      return node.as<T> ();
    }
    catch (YAML::Exception e) {
      throw (std::runtime_error ( "YAML::Exception when accessing path " 
                                + Util :: toString (path)
                                + ": " + e.what ()));
    }
  }
};

Config :: Config () : impl (new ConfigImpl ("dilay.config")) {}

DELEGATE_DESTRUCTOR (Config)
GLOBAL              (Config);

template <class T>
T Config :: get (const std::initializer_list <std::string>& path) const {
  return this->impl->get<T> (path);
}

typedef const std::initializer_list <std::string>& TMP;

template float Config :: get<float> (TMP) const;
template int   Config :: get<int>   (TMP) const;

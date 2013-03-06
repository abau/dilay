#ifndef DILAY_YAML
#define DILAY_YAML

#include <yaml-cpp/yaml.h>
#include "util.hpp"

class Yaml {
  public:   Yaml       () {}
            Yaml       (const Yaml&) = delete;
    Yaml&   operator=  (const Yaml&) = delete;

    void  initialize (const std::string& fileName) {
      this->_document = YAML::LoadFile (fileName);
    }

    template <class T>
    T get (const std::initializer_list <std::string>& path) {
      try {
        YAML::Node& node = this->_document;
        for (const std::string& p : path) {
            node = node [ p ];
        }
        return node.as<T> ();
      }
      catch (YAML::Exception e) {
        throw (std::runtime_error ( "YAML::Exception when accessing path " 
                                  + Util :: toString (path)
                                  + ": " + e.what ()));
      }
    }
  private:
    YAML::Node _document;
};

#endif

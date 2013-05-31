#include <yaml-cpp/yaml.h>
#include "color.hpp"

namespace YAML {
  template <> struct convert<Color> {

    static Node encode (const Color& color) {
      YAML::Node node;
      node.push_back(color.r ());
      node.push_back(color.g ());
      node.push_back(color.b ());
      return node;
    }
    static bool decode (const Node& node, Color& color) {
      if(!node.IsSequence() || node.size() != 3)
        return false;

      color = Color ( node[0].as<float>()
                    , node[1].as<double>()
                    , node[2].as<double>());
      return true;
    }
  };
}


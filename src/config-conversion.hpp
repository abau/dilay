#include <yaml-cpp/yaml.h>
#include <glm/glm.hpp>
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

  template <> struct convert<glm::vec3> {
    static Node encode (const glm::vec3& v) {
      YAML::Node node;
      node.push_back(v.x);
      node.push_back(v.y);
      node.push_back(v.z);
      return node;
    }
    static bool decode (const Node& node, glm::vec3& v) {
      if(!node.IsSequence() || node.size() != 3)
        return false;

      v.x = node[0].as<float>();
      v.y = node[1].as<float>();
      v.z = node[2].as<float>();
      return true;
    }
  };
}


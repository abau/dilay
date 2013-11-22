#include <fstream>
#include <sstream>
#include <glm/glm.hpp>
#include "util.hpp"

std::ostream& operator<<(std::ostream& os, const glm::ivec2& v) {
  os << "(" << v.x << "," << v.y << ")";
  return os;
}

std::ostream& operator<<(std::ostream& os, const glm::uvec2& v) {
  os << "(" << v.x << "," << v.y << ")";
  return os;
}

std::ostream& operator<<(std::ostream& os, const glm::vec3& v) {
  os << "(" << v.x << "," << v.y << "," << v.z << ")";
  return os;
}

std::ostream& operator<<(std::ostream& os, const glm::vec4& v) {
  os << "(" << v.x << "," << v.y << "," << v.z << "," << v.w << ")";
  return os;
}

glm::vec3 Util :: between (const glm::vec3& a, const glm::vec3& b) {
  return (a + b) * (glm::vec3 (0.5f));
}

std::string Util :: readFile (const std::string& filePath) {
  std::string   content;
  std::ifstream stream(filePath, std::ios::in);
  if(stream.is_open()) {
    std::string line ("");
    while(getline(stream, line))
      content += "\n" + line;
    stream.close();
  }
  return content;
}

unsigned int Util :: solveQuadraticEq ( float a, float b, float c
                                      , float& s1, float& s2) {
  float radicand = (b*b) - (4.0f * a * c);

  if (radicand < 0.0f)
    return 0;

  float root = glm::sqrt (radicand);

  if (b < 0.0f) {
    s1 = (-b + root) / (2.0f * a);
    s2 = (2.0f * c)  / (-b + root);
    return 2;
  }
  else if (b > 0.0f) {
    s1 = (-b - root) / (2.0f * a);
    s2 = (2.0f * c)  / (-b - root);
    return 2;
  }
  else {
    s1 = root / (2.0f * a);
    return 1;
  }
}

bool Util :: implies (bool p, bool c) {
  if (p && !c)
    return false;
  return true;
}

#include <fstream>
#include <sstream>
#include <glm/glm.hpp>
#include <QPoint>
#include <QMouseEvent>
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

glm::vec3 Util :: transformPosition (const glm::mat4x4& m, const glm::vec3& v) {
  return glm::vec3 (m * glm::vec4 (v, 1.0f));
}

glm::vec3 Util :: transformDirection (const glm::mat4x4& m, const glm::vec3& v) {
  return glm::vec3 (m * glm::vec4 (v, 0.0f));
}

glm::uvec2 Util :: toPoint (const QPoint& p) {
  return glm::uvec2 (p.x (), p.y ());
}

glm::uvec2 Util :: toPoint (const QMouseEvent& e) {
  return Util::toPoint (e.pos ());
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
  const float radicand = (b*b) - (4.0f * a * c);
  const float eps      = std::numeric_limits<float>::epsilon();

  if (radicand < eps)
    return 0;

  const float root = glm::sqrt (radicand);

  if (b < - eps) {
    s1 = (-b + root) / (2.0f * a);
    s2 = (2.0f * c)  / (-b + root);
    return 2;
  }
  else if (b > eps) {
    s1 = (-b - root) / (2.0f * a);
    s2 = (2.0f * c)  / (-b - root);
    return 2;
  }
  else {
    s1 = root / (2.0f * a);
    return 1;
  }
}

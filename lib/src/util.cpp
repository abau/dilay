#include <fstream>
#include <glm/glm.hpp>
#include <sstream>
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

unsigned int Util :: solveQuadraticEq (float a, float b, float c, float& s1, float& s2) {
  const float radicand = (b*b) - (4.0f * a * c);

  if (radicand < Util::epsilon ())
    return 0;

  const float root = glm::sqrt (radicand);

  if (b < - Util::epsilon ()) {
    s1 = (-b + root) / (2.0f * a);
    s2 = (2.0f * c)  / (-b + root);
    return 2;
  }
  else if (b > Util::epsilon ()) {
    s1 = (-b - root) / (2.0f * a);
    s2 = (2.0f * c)  / (-b - root);
    return 2;
  }
  else {
    s1 = root / (2.0f * a);
    return 1;
  }
}

float Util :: epsilon () {
  return 0.0001f;
}

bool Util :: isNaN (const glm::vec3& v) {
  return glm::isnan (v.x) || glm::isnan (v.y) || glm::isnan (v.z);
}

float Util :: defaultScale () {
  return 100.0f;
}

float Util :: smoothStep ( const glm::vec3& v, const glm::vec3& center
                         , float innerRadius, float radius )
{
  assert (innerRadius <= radius);

  const float d = glm::distance <float> (v, center);

  if (radius - innerRadius < Util::epsilon ()) {
    return d > radius ? 0.0f : 1.0f;
  }
  else {
    const float x = glm::clamp ((radius - d) / (radius - innerRadius), 0.0f, 1.0f);
    return x*x*x * (x * (x*6.0f - 15.0f) + 10.0f);
  }
}

float Util :: linearStep ( const glm::vec3& v, const glm::vec3& center
                         , float innerRadius, float radius )
{
  assert (innerRadius <= radius);

  const float d = glm::distance <float> (v, center);

  if (radius - innerRadius < Util::epsilon ()) {
    return d > radius ? 0.0f : 1.0f;
  }
  else {
    const float x = glm::clamp ((radius - d) / (radius - innerRadius), 0.0f, 1.0f);
    return glm::mix (0.0f, 1.0f, x);
  }
}

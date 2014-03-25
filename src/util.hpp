#ifndef DILAY_UTIL
#define DILAY_UTIL

#include <iosfwd>
#include <initializer_list>
#include <glm/fwd.hpp>

class QPoint;

std::ostream& operator<<(std::ostream&, const glm::ivec2&);
std::ostream& operator<<(std::ostream&, const glm::uvec2&);
std::ostream& operator<<(std::ostream&, const glm::vec3&);
std::ostream& operator<<(std::ostream&, const glm::vec4&);

namespace Util {
  template <class T> std::string toString (const T&);

  template <class T> std::string toString (const std::initializer_list<T>&);

  glm::vec3  between            (const glm::vec3&, const glm::vec3&);
  glm::vec3  transformPosition  (const glm::mat4x4&, const glm::vec3&);
  glm::vec3  transformDirection (const glm::mat4x4&, const glm::vec3&);
  glm::uvec2 toPoint            (const QPoint&);

  std::string readFile (const std::string&); 

  unsigned int solveQuadraticEq (float, float, float, float&, float&);
}

#endif

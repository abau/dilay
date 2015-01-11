#ifndef DILAY_UTIL
#define DILAY_UTIL

#include <glm/fwd.hpp>
#include <iosfwd>

std::ostream& operator<<(std::ostream&, const glm::ivec2&);
std::ostream& operator<<(std::ostream&, const glm::uvec2&);
std::ostream& operator<<(std::ostream&, const glm::vec3&);
std::ostream& operator<<(std::ostream&, const glm::vec4&);

namespace Util {
  glm::vec3       between            (const glm::vec3&, const glm::vec3&);
  glm::vec3       transformPosition  (const glm::mat4x4&, const glm::vec3&);
  glm::vec3       transformDirection (const glm::mat4x4&, const glm::vec3&);
  std::string     readFile           (const std::string&); 
  unsigned int    solveQuadraticEq   (float, float, float, float&, float&);
  float           epsilon            ();
  bool            isNaN              (const glm::vec3&);
  float           defaultScale       ();
}

#endif

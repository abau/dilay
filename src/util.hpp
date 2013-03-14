#ifndef DILAY_UTIL
#define DILAY_UTIL

#include <iosfwd>
#include <initializer_list>
#include "fwd-glm.hpp"

std::ostream& operator<<(std::ostream&, const glm::vec3&);

namespace Util {
  const float epsilon = 0.000001f;

  template <class T> std::string toString (const T&);

  template <class T> std::string toString (const std::initializer_list<T>&);

  glm::vec3 between (const glm::vec3&, const glm::vec3&);

  std::string readFile (const std::string&); 
}

#endif

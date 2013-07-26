#ifndef DILAY_UTIL
#define DILAY_UTIL

#include <iosfwd>
#include <initializer_list>
#include <list>
#include "fwd-glm.hpp"

std::ostream& operator<<(std::ostream&, const glm::ivec2&);
std::ostream& operator<<(std::ostream&, const glm::uvec2&);
std::ostream& operator<<(std::ostream&, const glm::vec3&);
std::ostream& operator<<(std::ostream&, const glm::vec4&);

namespace Util {
  const float epsilon = 0.000001f;

  template <class T> std::string toString (const T&);

  template <class T> std::string toString (const std::initializer_list<T>&);

  glm::vec3 between (const glm::vec3&, const glm::vec3&);

  std::string readFile (const std::string&); 

  unsigned int solveQuadraticEq (float, float, float, float&, float&);

  template <class T> 
  void eraseByAddress (std::list <T>& list, const T* element) {
    for (typename std::list<T>::iterator it = list.begin (); it != list.end (); ++it) {
      if (&*it == element) {
        list.erase (it);
        return;
      }
    }
    assert (false);
  }
}

#endif

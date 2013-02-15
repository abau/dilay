#include <sstream>
#include <glm/glm.hpp>

#ifndef UTIL
#define UTIL

#define STRING_ID(x)    ((x) == 0 ? std::string ("NULL") : Util::toString ((x)->data ().id ()))
#define STRING_INDEX(x) ((x) == 0 ? std::string ("NULL") : Util::toString ((x)->data ().index ()))

#define PRIVATE_ASSIGNMENT_OP(x) const x & operator=(const x &);

template <class T> class LinkedElement;
class WingedVertex;
class WingedEdge;
class WingedFace;

typedef LinkedElement <WingedVertex> LinkedVertex;
typedef LinkedElement <WingedEdge>   LinkedEdge;
typedef LinkedElement <WingedFace>   LinkedFace;

std::ostream& operator<<(std::ostream&, const glm::vec3&);

namespace Util {
  const float epsilon = 0.000001;

  template <class T>
  std::string toString (const T& t) {
    std::stringstream ss;
    ss << t;
    return ss.str ();
  }

  glm::vec3 between (const glm::vec3&, const glm::vec3&);

  template <class T> void swap (T& a, T& b) {
    T tmp (a);
    a = b;
    b = tmp;
  }

  std::string readFile (const char*); 
}

#endif

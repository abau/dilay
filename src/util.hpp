#ifndef UTIL
#define UTIL

#include <sstream>
#include <glm/glm.hpp>
#include <list>

class WingedVertex;
class WingedEdge;
class WingedFace;
class WingedMesh;

typedef std::list<WingedVertex>::iterator               LinkedVertex;
typedef std::list<WingedEdge>  ::iterator               LinkedEdge;
typedef std::list<WingedFace>  ::iterator               LinkedFace;

typedef std::list<WingedVertex>::const_iterator         ConstLinkedVertex;
typedef std::list<WingedEdge>  ::const_iterator         ConstLinkedEdge;
typedef std::list<WingedFace>  ::const_iterator         ConstLinkedFace;

std::ostream& operator<<(std::ostream&, const glm::vec3&);

namespace Util {
  const float epsilon = 0.000001f;

  template <class T>
  std::string toString (const T& t) {
    std::stringstream ss;
    ss << t;
    return ss.str ();
  }

  template <class T>
  std::string toString (const std::initializer_list<T>& ts) {
    std::stringstream ss;
    bool isFirst = true;
    ss << "{ ";
    for (const T& t : ts) {
      if (isFirst)
        isFirst = ! isFirst;
      else
        ss << ", ";
      ss << t;
    }
    ss << " }";
    return ss.str ();
  }

  glm::vec3 between (const glm::vec3&, const glm::vec3&);

  std::string readFile (const char*); 
}

#endif

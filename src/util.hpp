#ifndef UTIL
#define UTIL

#include <sstream>
#include <glm/glm.hpp>
#include <list>

#define STRING_ID(x)    ((x) == 0 ? std::string ("NULL") : Util::toString ((x)->id ()))
#define STRING_INDEX(x) ((x) == 0 ? std::string ("NULL") : Util::toString ((x)->index ()))

class WingedVertex;
class WingedEdge;
class WingedFace;
class WingedMesh;

typedef std::list<WingedVertex>::iterator       LinkedVertex;
typedef std::list<WingedEdge>  ::iterator       LinkedEdge;
typedef std::list<WingedFace>  ::iterator       LinkedFace;

typedef std::list<WingedVertex>::const_iterator ConstLinkedVertex;
typedef std::list<WingedEdge>  ::const_iterator ConstLinkedEdge;
typedef std::list<WingedFace>  ::const_iterator ConstLinkedFace;

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

  std::string readFile (const char*); 
}

#endif

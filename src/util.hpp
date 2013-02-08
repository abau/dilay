#include <sstream>
#include "glm/glm.hpp"

#ifndef UTIL
#define UTIL

#define STRING_ID(x)    ((x) == 0 ? std::string ("NULL") : Util::toString ((x)->data ().id ()))
#define STRING_INDEX(x) ((x) == 0 ? std::string ("NULL") : Util::toString ((x)->data ().index ()))

// Use CONSTi macros with care
#define CONST0(resultT,method) \
  const resultT method () const { \
    return const_cast < resultT > ( method () ) ; \
  }

#define CONST1(resultT,method,arg1T,arg1) \
  const resultT method (arg1T arg1) const { \
    return const_cast < resultT > ( method (arg1) ) ; \
  }

#define CONST2(resultT,method,arg1T,arg1,arg2T,arg2) \
  const resultT method (arg1T arg1,arg2T arg2) const { \
    return const_cast < resultT > ( method (arg1,arg2) ) ; \
  }

#define CONST3(resultT,method,arg1T,arg1,arg2T,arg2,arg3T,arg3) \
  const resultT method (arg1T arg1,arg2T arg2,arg3T arg3) const { \
    return const_cast < resultT > ( method (arg1,arg2,arg3) ) ; \
  }

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

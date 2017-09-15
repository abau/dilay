/* This file is part of Dilay
 * Copyright © 2015-2017 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_UTIL
#define DILAY_UTIL

#include <algorithm>
#include <cstdlib>
#include <functional>
#include <glm/fwd.hpp>
#include <iosfwd>
#include <limits>
#include <utility>
#include <vector>

#define DILAY_WARN(fmt, ...) Util::warn (__FILE__, __LINE__, fmt, ##__VA_ARGS__);
#define DILAY_PANIC(fmt, ...)                                             \
  {                                                                       \
    Util::warn (__FILE__, __LINE__, fmt, ##__VA_ARGS__);                  \
    Util::warn (__FILE__, __LINE__, "aborting due to previous error..."); \
    std::abort ();                                                        \
  }
#define DILAY_IMPOSSIBLE DILAY_PANIC ("the impossible happend")

#ifndef NDEBUG
#define DILAY_WARN_DEBUG(fmt, ...) DILAY_WARN (fmt, ##__VA_ARGS__);
#else
#define DILAY_WARN_DEBUG(fmt, ...) (void) 0;
#endif

std::ostream& operator<< (std::ostream&, const glm::vec2&);
std::ostream& operator<< (std::ostream&, const glm::vec3&);
std::ostream& operator<< (std::ostream&, const glm::vec4&);
std::istream& operator>> (std::istream&, glm::vec3&);

typedef std::pair<unsigned int, unsigned int> ui_pair;

namespace Util
{
  glm::vec3    midpoint (const glm::vec3&, const glm::vec3&);
  glm::vec3    transformPosition (const glm::mat4x4&, const glm::vec3&);
  glm::vec3    transformDirection (const glm::mat4x4&, const glm::vec3&);
  glm::vec3    orthogonal (const glm::vec3&);
  bool         colinear (const glm::vec2&, const glm::vec2&);
  bool         colinear (const glm::vec3&, const glm::vec3&);
  bool         colinearUnit (const glm::vec2&, const glm::vec2&);
  bool         colinearUnit (const glm::vec3&, const glm::vec3&);
  float        smoothStep (const glm::vec3&, const glm::vec3&, float, float);
  float        linearStep (const glm::vec3&, const glm::vec3&, float, float);
  std::string  readFile (const std::string&);
  unsigned int solveQuadraticEq (float, float, float, float&, float&);
  unsigned int solveCubicEq (float, float, float, float&, float&, float&);
  unsigned int solveCubicEq (float, float, float, float, float&, float&, float&);
  bool         isNaN (float);
  bool         isNaN (const glm::vec3&);
  bool         isNotNull (const glm::vec3&);
  bool         almostEqual (float, float);
  void         warn (const char*, unsigned int, const char*, ...);
  bool         fromString (const std::string&, int&);
  bool         fromString (const std::string&, unsigned int&);
  bool         fromString (const std::string&, float&);
  unsigned int countOnes (unsigned int);
  bool         hasSuffix (const std::string&, const std::string&);
  void         setCLocale ();
  void         setSystemLocale ();

  constexpr float epsilon () { return 0.0001f; }

  constexpr float minFloat () { return std::numeric_limits<float>::lowest (); }

  constexpr float maxFloat () { return std::numeric_limits<float>::max (); }

  constexpr int minInt () { return std::numeric_limits<int>::lowest (); }

  constexpr int maxInt () { return std::numeric_limits<int>::max (); }

  constexpr int maxUnsignedInt () { return std::numeric_limits<unsigned int>::max (); }

  constexpr unsigned int invalidIndex () { return std::numeric_limits<unsigned int>::max (); }

  template <typename T> void setIfNotNull (T* ptr, const T& value)
  {
    if (ptr)
    {
      *ptr = value;
    }
  }

  template <typename T> unsigned int findIndexByReference (const std::vector<T>& vec, const T& obj)
  {
    return &obj - &vec.at (0);
  }

  template <typename T> T withCLocale (const std::function<T ()>& f)
  {
    setCLocale ();
    T result = f ();
    setSystemLocale ();
    return result;
  }

  template <> void withCLocale (const std::function<void()>& f);

  template <typename T>
  void prune (std::vector<T>& v, const std::function<bool(const T&)>& p,
              std::vector<unsigned int>* indexMap = nullptr)
  {
    if (indexMap)
    {
      indexMap->resize (v.size (), Util::invalidIndex ());
    }
    auto lastIt = std::find_if_not (v.rbegin (), v.rend (), p);

    if (lastIt == v.rend ())
    {
      v.clear ();
    }
    else
    {
      unsigned int last = v.size () - (lastIt - v.rbegin ()) - 1;

      for (unsigned int i = 0; i <= last; i++)
      {
        if (p (v[i]))
        {
          v[i] = v[last];

          if (indexMap)
          {
            (*indexMap)[i] = Util::invalidIndex ();
            (*indexMap)[last] = i;
          }

          do
          {
            last--;
          } while (p (v[last]));
        }
        else
        {
          if (indexMap)
          {
            (*indexMap)[i] = i;
          }
        }
      }
      v.resize (last + 1);
    }
  }
}

#endif

/* This file is part of Dilay
 * Copyright © 2015 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_UTIL
#define DILAY_UTIL

#include <glm/fwd.hpp>
#include <iosfwd>
#include <limits>
#include <utility>

#define DILAY_WARN(fmt, ...)  Util::warn (__FILE__, __LINE__, fmt, ##__VA_ARGS__);
#define DILAY_PANIC(fmt, ...) { Util::warn (__FILE__, __LINE__, fmt, ##__VA_ARGS__); \
                                Util::warn (__FILE__, __LINE__, "aborting due to previous error..."); \
                                std::abort (); }
#define DILAY_IMPOSSIBLE DILAY_PANIC ("the impossible happend")

std::ostream& operator<<(std::ostream&, const glm::ivec2&);
std::ostream& operator<<(std::ostream&, const glm::uvec2&);
std::ostream& operator<<(std::ostream&, const glm::vec3&);
std::ostream& operator<<(std::ostream&, const glm::vec4&);

typedef std::pair <unsigned int, unsigned int> ui_pair;

namespace Util {
  glm::vec3    between            (const glm::vec3&, const glm::vec3&);
  glm::vec3    transformPosition  (const glm::mat4x4&, const glm::vec3&);
  glm::vec3    transformDirection (const glm::mat4x4&, const glm::vec3&);
  glm::vec3    orthogonal         (const glm::vec3&);
  bool         colinear           (const glm::vec3&, const glm::vec3&);
  std::string  readFile           (const std::string&); 
  unsigned int solveQuadraticEq   (float, float, float, float&, float&);
  bool         isNaN              (const glm::vec3&);
  void         warn               (const char*, unsigned int, const char*, ...);
  bool         fromString         (const std::string&, int&);
  bool         fromString         (const std::string&, unsigned int&);
  bool         fromString         (const std::string&, float&);

  constexpr float        epsilon      () { return 0.0001f; }
  constexpr unsigned int invalidIndex () { return std::numeric_limits <unsigned int>::max (); }
}

#endif

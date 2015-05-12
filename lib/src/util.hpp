#ifndef DILAY_UTIL
#define DILAY_UTIL

#include <glm/fwd.hpp>
#include <iosfwd>
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
  std::string  readFile           (const std::string&); 
  unsigned int solveQuadraticEq   (float, float, float, float&, float&);
  bool         isNaN              (const glm::vec3&);
  float        smoothStep         (const glm::vec3&, const glm::vec3&, float, float);
  float        linearStep         (const glm::vec3&, const glm::vec3&, float, float);
  void         warn               (const char*, unsigned int, const char*, ...);

  constexpr float epsilon      () { return 0.0001f; }
  constexpr float defaultScale () { return 100.0f; }
}

#endif

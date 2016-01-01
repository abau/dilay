/* This file is part of Dilay
 * Copyright © 2015,2016 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <cstdarg>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/epsilon.hpp>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <vector>
#include "util.hpp"

std::ostream& operator<<(std::ostream& os, const glm::ivec2& v) {
  os << v.x << " " << v.y;
  return os;
}

std::ostream& operator<<(std::ostream& os, const glm::uvec2& v) {
  os << v.x << " " << v.y;
  return os;
}

std::ostream& operator<<(std::ostream& os, const glm::vec3& v) {
  os << v.x << " " << v.y << " " << v.z;
  return os;
}

std::ostream& operator<<(std::ostream& os, const glm::vec4& v) {
  os << v.x << " " << v.y << " " << v.z << " " << v.w;
  return os;
}

std::istream& operator>>(std::istream& is, glm::vec3& v) {
  is >> v.x >> v.y >> v.z;
  return is;
}

glm::vec3 Util :: between (const glm::vec3& a, const glm::vec3& b) {
  return (a + b) * (glm::vec3 (0.5f));
}

glm::vec3 Util :: transformPosition (const glm::mat4x4& m, const glm::vec3& v) {
  return glm::vec3 (m * glm::vec4 (v, 1.0f));
}

glm::vec3 Util :: transformDirection (const glm::mat4x4& m, const glm::vec3& v) {
  return glm::vec3 (m * glm::vec4 (v, 0.0f));
}

glm::vec3 Util :: orthogonal (const glm::vec3& v) {
  return glm::abs (v.x) > glm::abs (v.z) ? glm::vec3 (-v.y, v.x, 0.0f)
                                         : glm::vec3 (0.0f, -v.z, v.y);
}

bool Util :: colinear (const glm::vec3& v1, const glm::vec3& v2) {
  return Util::colinearUnit (glm::normalize (v1), glm::normalize (v2));
}

bool Util :: colinearUnit (const glm::vec3& v1, const glm::vec3& v2) {
  return glm::epsilonEqual (glm::abs (glm::dot (v1, v2)), 1.0f, Util::epsilon ());
}

float Util :: smoothStep ( const glm::vec3& v, const glm::vec3& center
                         , float innerRadius, float radius )
{
  assert (innerRadius <= radius);

  const float d = glm::distance <float> (v, center);

  if (radius - innerRadius < Util::epsilon ()) {
    return d > radius ? 0.0f : 1.0f;
  }
  else {
    const float x = glm::clamp ((radius - d) / (radius - innerRadius), 0.0f, 1.0f);
    return x*x*x * (x * (x*6.0f - 15.0f) + 10.0f);
  }
}

float Util :: linearStep ( const glm::vec3& v, const glm::vec3& center
                         , float innerRadius, float radius)
{
  assert (innerRadius <= radius);

  const float d = glm::distance <float> (v, center);

  if (radius - innerRadius < Util::epsilon ()) {
    return d > radius ? 0.0f : 1.0f;
  }
  else {
    return glm::clamp ((radius - d) / (radius - innerRadius), 0.0f, 1.0f);
  }
}

std::string Util :: readFile (const std::string& filePath) {
  std::string   content;
  std::ifstream stream(filePath, std::ios::in);
  if(stream.is_open()) {
    std::string line ("");
    while(getline(stream, line))
      content += "\n" + line;
    stream.close();
  }
  return content;
}

unsigned int Util :: solveQuadraticEq (float a, float b, float c, float& s1, float& s2) {
  const float radicand = (b*b) - (4.0f * a * c);

  if (radicand < Util::epsilon ())
    return 0;

  const float root = glm::sqrt (radicand);

  if (b < - Util::epsilon ()) {
    s1 = (-b + root) / (2.0f * a);
    s2 = (2.0f * c)  / (-b + root);
    return 2;
  }
  else if (b > Util::epsilon ()) {
    s1 = (-b - root) / (2.0f * a);
    s2 = (2.0f * c)  / (-b - root);
    return 2;
  }
  else {
    s1 = root / (2.0f * a);
    return 1;
  }
}

bool Util :: isNaN (const glm::vec3& v) {
  return glm::any (glm::isnan (v));
}

bool Util :: isNotNull (const glm::vec3& v) {
  return glm::any (glm::greaterThan (v, glm::vec3 (Util::epsilon ())));
}

void Util :: warn (const char* file, unsigned int line, const char* format, ...) {
  va_list args1;
  va_list args2;

  va_start (args1, format);
  va_copy  (args2, args1);

  std::vector<char> buffer (1 + std::vsnprintf (nullptr, 0, format, args1));
  va_end (args1);

  std::vsnprintf (buffer.data (), buffer.size (), format, args2);
  va_end (args2);

  std::fprintf (stderr, "[WARNING] %s (%u): %s\n", file, line, buffer.data ());
}

bool Util :: fromString (const std::string& string, int& value) {
  try {
    value = std::stoi (string);
    return true;
  }
  catch (const std::invalid_argument&) { return false; }
  catch (const std::out_of_range&)     { return false; }
}

bool Util :: fromString (const std::string& string, unsigned int& value) {
  try {
    value = (unsigned int) (std::stoul (string));
    return true;
  }
  catch (const std::invalid_argument&) { return false; }
  catch (const std::out_of_range&)     { return false; }
}

bool Util :: fromString (const std::string& string, float& value) {
  try {
    value = std::stof (string);
    return true;
  }
  catch (const std::invalid_argument&) { return false; }
  catch (const std::out_of_range&)     { return false; }
}

unsigned int Util :: countOnes (unsigned int x) {
  unsigned int n = 0;
  for (; x > 0; n++) {
    x &= x-1;
  }
  return n;
}

bool Util :: hasSuffix (const std::string& string, const std::string& suffix) {
  if (string.size () >= suffix.size ()) {
    return string.compare (string.size () - suffix.size (), suffix.size (), suffix) == 0;
  }
  else {
    return false;
  }
}

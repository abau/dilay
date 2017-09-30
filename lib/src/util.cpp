/* This file is part of Dilay
 * Copyright © 2015-2017 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/epsilon.hpp>
#include <iostream>
#include <locale>
#include <sstream>
#include <stdexcept>
#include <vector>
#include "util.hpp"

namespace
{
  template <typename T> bool colinearUnitT (const T& v1, const T& v2)
  {
    return Util::almostEqual (glm::abs (glm::dot (v1, v2)), 1.0f);
  }

  template <typename T> bool colinearT (const T& v1, const T& v2)
  {
    return colinearUnitT<T> (glm::normalize (v1), glm::normalize (v2));
  }
}

std::ostream& operator<< (std::ostream& os, const glm::vec2& v)
{
  os << v.x << " " << v.y;
  return os;
}

std::ostream& operator<< (std::ostream& os, const glm::vec3& v)
{
  os << v.x << " " << v.y << " " << v.z;
  return os;
}

std::ostream& operator<< (std::ostream& os, const glm::vec4& v)
{
  os << v.x << " " << v.y << " " << v.z << " " << v.w;
  return os;
}

std::istream& operator>> (std::istream& is, glm::vec3& v)
{
  is >> v.x >> v.y >> v.z;
  return is;
}

glm::vec3 Util::midpoint (const glm::vec3& a, const glm::vec3& b)
{
  return (a + b) * (glm::vec3 (0.5f));
}

glm::vec3 Util::transformPosition (const glm::mat4x4& m, const glm::vec3& v)
{
  return glm::vec3 (m * glm::vec4 (v, 1.0f));
}

glm::vec3 Util::transformDirection (const glm::mat4x4& m, const glm::vec3& v)
{
  return glm::vec3 (m * glm::vec4 (v, 0.0f));
}

glm::vec3 Util::orthogonal (const glm::vec3& v)
{
  return glm::abs (v.x) > glm::abs (v.z) ? glm::vec3 (-v.y, v.x, 0.0f)
                                         : glm::vec3 (0.0f, -v.z, v.y);
}

glm::ivec2 Util::orthogonalRight (const glm::ivec2& v) { return glm::ivec2 (v.y, -v.x); }

bool Util::colinear (const glm::vec2& v1, const glm::vec2& v2)
{
  return colinearT<glm::vec2> (v1, v2);
}

bool Util::colinear (const glm::vec3& v1, const glm::vec3& v2)
{
  return colinearT<glm::vec3> (v1, v2);
}

bool Util::colinearUnit (const glm::vec2& v1, const glm::vec2& v2)
{
  return colinearUnitT<glm::vec2> (v1, v2);
}

bool Util::colinearUnit (const glm::vec3& v1, const glm::vec3& v2)
{
  return colinearUnitT<glm::vec3> (v1, v2);
}

float Util::smoothStep (const glm::vec3& v, const glm::vec3& center, float innerRadius,
                        float radius)
{
  assert (innerRadius <= radius);

  const float d = glm::distance<float> (v, center);

  if (radius - innerRadius < Util::epsilon ())
  {
    return d > radius ? 0.0f : 1.0f;
  }
  else
  {
    const float x = glm::clamp ((radius - d) / (radius - innerRadius), 0.0f, 1.0f);
    return x * x * x * (x * (x * 6.0f - 15.0f) + 10.0f);
  }
}

float Util::linearStep (const glm::vec3& v, const glm::vec3& center, float innerRadius,
                        float radius)
{
  assert (innerRadius <= radius);

  const float d = glm::distance<float> (v, center);

  if (radius - innerRadius < Util::epsilon ())
  {
    return d > radius ? 0.0f : 1.0f;
  }
  else
  {
    return glm::clamp ((radius - d) / (radius - innerRadius), 0.0f, 1.0f);
  }
}

float Util::cross (const glm::vec2& v1, const glm::vec2& v2)
{
  return (v1.x * v2.y) - (v1.y * v2.x);
}

std::string Util::readFile (const std::string& filePath)
{
  std::string   content;
  std::ifstream stream (filePath, std::ios::in);
  if (stream.is_open ())
  {
    std::string line ("");
    while (getline (stream, line))
      content += "\n" + line;
    stream.close ();
  }
  return content;
}

unsigned int Util::solveQuadraticEq (float a, float b, float c, float& s1, float& s2)
{
  const float radicand = (b * b) - (4.0f * a * c);

  if (radicand < Util::epsilon ())
    return 0;

  const float root = glm::sqrt (radicand);

  if (b < -Util::epsilon ())
  {
    s1 = (-b + root) / (2.0f * a);
    s2 = (2.0f * c) / (-b + root);
    return 2;
  }
  else if (b > Util::epsilon ())
  {
    s1 = (-b - root) / (2.0f * a);
    s2 = (2.0f * c) / (-b - root);
    return 2;
  }
  else
  {
    s1 = root / (2.0f * a);
    return 1;
  }
}

unsigned int Util::solveCubicEq (float a, float b, float c, float d, float& s1, float& s2,
                                 float& s3)
{
  return Util::solveCubicEq (b / a, c / a, d / a, s1, s2, s3);
}

unsigned int Util::solveCubicEq (float a, float b, float c, float& s1, float& s2, float& s3)
{
  const float q = ((a * a) - (3.0f * b)) / 9.0f;
  const float r = ((2.0f * a * a * a) - (9.0f * a * b) + (27.0f * c)) / 54.0f;
  const float w = (r * r) - (q * q * q);
  const float s = a / 3.0f;

  if (w < 0.0f)
  {
    const float theta = glm::acos (r / glm::sqrt (q * q * q));
    const float f = -2.0f * glm::sqrt (q);

    s1 = (f * glm::cos (theta / 3.0f)) - s;
    s2 = (f * glm::cos ((theta + glm::two_pi<float> ()) / 3.0f)) - s;
    s3 = (f * glm::cos ((theta - glm::two_pi<float> ()) / 3.0f)) - s;
    return 3;
  }
  else
  {
    const float x = -glm::sign (r) * glm::pow (glm::abs (r) + glm::sqrt (w), 1.0f / 3.0f);
    const float y = Util::almostEqual (x, 0.0f) ? 0.0f : q / x;

    s1 = x + y - s;
    return 1;
  }
}

bool Util::isNaN (float v) { return glm::isnan (v); }

bool Util::isNaN (const glm::vec3& v) { return glm::any (glm::isnan (v)); }

bool Util::isNotNull (const glm::vec3& v)
{
  const bool bx = Util::almostEqual (v.x, 0.0f) == false;
  const bool by = Util::almostEqual (v.y, 0.0f) == false;
  const bool bz = Util::almostEqual (v.z, 0.0f) == false;

  return bx || by || bz;
}

bool Util::almostEqual (float a, float b) { return glm::epsilonEqual (a, b, Util::epsilon ()); }

bool Util::fromString (const std::string& string, int& value)
{
  try
  {
    value = std::stoi (string);
    return true;
  }
  catch (const std::invalid_argument&)
  {
    return false;
  }
  catch (const std::out_of_range&)
  {
    return false;
  }
}

bool Util::fromString (const std::string& string, unsigned int& value)
{
  try
  {
    value = (unsigned int) (std::stoul (string));
    return true;
  }
  catch (const std::invalid_argument&)
  {
    return false;
  }
  catch (const std::out_of_range&)
  {
    return false;
  }
}

bool Util::fromString (const std::string& string, float& value)
{
  try
  {
    value = std::stof (string);
    return true;
  }
  catch (const std::invalid_argument&)
  {
    return false;
  }
  catch (const std::out_of_range&)
  {
    return false;
  }
}

unsigned int Util::countOnes (unsigned int x)
{
  unsigned int n = 0;
  for (; x > 0; n++)
  {
    x &= x - 1;
  }
  return n;
}

bool Util::hasSuffix (const std::string& string, const std::string& suffix)
{
  if (string.size () >= suffix.size ())
  {
    return string.compare (string.size () - suffix.size (), suffix.size (), suffix) == 0;
  }
  else
  {
    return false;
  }
}

void Util::setCLocale () { std::locale::global (std::locale::classic ()); }

void Util::setSystemLocale () { std::locale::global (std::locale ("")); }

namespace Util
{
  template <> void withCLocale (const std::function<void()>& f)
  {
    setCLocale ();
    f ();
    setSystemLocale ();
  }
}

#include <fstream>
#include <sstream>
#include <glm/glm.hpp>
#include "util.hpp"

std::ostream& operator<<(std::ostream& os, const glm::vec3& v) {
  os << "(" << v.x << "," << v.y << "," << v.z << ")";
  return os;
}

std::ostream& operator<<(std::ostream& os, const glm::vec4& v) {
  os << "(" << v.x << "," << v.y << "," << v.z << "," << v.w << ")";
  return os;
}

template <class T>
std::string Util :: toString (const T& t) {
  std::stringstream ss;
  ss << t;
  return ss.str ();
}

template <class T>
std::string Util :: toString (const std::initializer_list<T>& ts) {
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

glm::vec3 Util :: between (const glm::vec3& a, const glm::vec3& b) {
  return (a + b) * (glm::vec3 (0.5f));
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

// Explicit instances
template std::string Util :: toString (const unsigned int&);
template std::string Util :: toString (const unsigned long&);
template std::string Util :: toString (const std::initializer_list<std::string>&);

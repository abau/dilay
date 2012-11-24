#include "ray.hpp"
#include "util.hpp"

std::ostream& operator<<(std::ostream& os, const Ray& ray) {
  os << "Ray { origin = "    << ray.origin ()    << ", " 
     <<       "direction = " << ray.direction () << " }";
  return os;
}

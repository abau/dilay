#include "depth.hpp"

std::ostream& operator<<(std::ostream& os, const Depth& v) { 
  os << v.value ();
  return os;
}

#include <fstream>
#include "id.hpp"

IdPrimitive IdObject::_currentIdPrimitive = Id ().primitive ();

std::ostream& operator<<(std::ostream& os, const Id& id) {
  os << id.primitive ();
  return os;
}

IdObject :: IdObject (const Id& id) 
  : _id (id.isInvalid () ? IdObject::nextIdPrimitive () : id.primitive ()) 
  {}

IdObject :: IdObject (const IdObject&) 
  : _id (IdObject::nextIdPrimitive ()) 
  {}

IdPrimitive IdObject :: nextIdPrimitive () {
  IdObject :: _currentIdPrimitive++;
  return IdObject :: _currentIdPrimitive;
}

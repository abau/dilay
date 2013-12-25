#include <fstream>
#include "id.hpp"

IdPrimitive IdObject::_currentIdPrimitive = Id ().get ();

std::ostream& operator<<(std::ostream& os, const Id& id) {
  os << id.get ();
  return os;
}

IdObject :: IdObject (const Id& id) 
  : _id (id.isInvalid () ? IdObject::nextId () : id.get ()) {}

IdObject :: IdObject (const IdObject&) : _id (IdObject::nextId ()) {}

IdPrimitive IdObject :: nextId () {
  IdObject :: _currentIdPrimitive++;
  return IdObject :: _currentIdPrimitive;
}

#include <fstream>
#include "id.hpp"

IdPrimitive IdObject::_currentId = IdObject::invalidId ().get ();

std::ostream& operator<<(std::ostream& os, const Id& id) {
  os << id.get ();
  return os;
}

IdObject :: IdObject (const Id& id) 
  : _id (id == IdObject::invalidId () ? IdObject::nextId () : id.get ()) {}

IdObject :: IdObject (const IdObject&) : _id (IdObject::nextId ()) {}

IdPrimitive IdObject :: nextId () {
  IdObject :: _currentId++;
  return IdObject :: _currentId++;
}

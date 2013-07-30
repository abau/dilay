#include <fstream>
#include "id.hpp"

std::ostream& operator<<(std::ostream& os, const Id& id) {
  os << id.get ();
  return os;
}

IdObject :: IdObject (const Id& id) 
  : _id (id == IdObject::invalidId () ? IdObject::nextId () : id.get ()) {}

IdObject :: IdObject (const IdObject&) : _id (IdObject::nextId ()) {}

IdPrimitive IdObject :: nextId () {
  static IdPrimitive nextId = IdObject::invalidId ().get ();
  nextId++;
  return nextId;
}


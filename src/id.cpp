#include "id.hpp"

Id :: Id (IdType id) : _id (id == Id::invalidId () ? Id::nextId () : id) {}
Id :: Id (const Id&) : _id (Id::nextId ()) {}

const Id& Id :: operator= (const Id&) {
  return *this;
}

IdType Id :: nextId () {
  static IdType nextId = Id::invalidId ();
  nextId++;
  return nextId;
}


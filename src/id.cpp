#include "id.hpp"

Id :: Id ()            : _id (Id::nextId ()) {}
Id :: Id (const Id& _) : _id (Id::nextId ()) {}

const Id& Id :: operator= (const Id& _) {
  return *this;
}

IdType Id :: nextId () {
  static IdType nextId = -1;
  nextId++;
  return nextId;
}


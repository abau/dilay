#include "id.hpp"

Id :: Id ()          : _id (Id::nextId ()) {}
Id :: Id (const Id&) : _id (Id::nextId ()) {}

const Id& Id :: operator= (const Id&) {
  return *this;
}

IdType Id :: nextId () {
  static IdType nextId = -1;
  nextId++;
  return nextId;
}


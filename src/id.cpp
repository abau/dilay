#include "id.hpp"

IdType Id :: nextId = 0;

Id :: Id () : _id (nextId) {
  nextId = nextId + 1;
}


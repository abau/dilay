#include "test-indexable.hpp"
#include "indexable.hpp"

class Foo {
  public:
    Foo (unsigned int, bool) {}
};

void TestIndexable::test () {

  IndexableList <Foo> foos;

  foos.emplace (false);

  assert (foos.numElements () == 1);
  assert (foos.numIndices () == 1);
  assert (foos.numFreeIndices () == 0);

  foos.reserveIndices (10);

  assert (foos.numElements () == 1);
  assert (foos.numIndices () == 11);
  assert (foos.numFreeIndices () == 10);

  foos.deleteIndex (0);

  assert (foos.numElements () == 0);
  assert (foos.numIndices () == 11);
  assert (foos.numFreeIndices () == 11);

  foos.emplace (true);

  assert (foos.numElements () == 1);
  assert (foos.numIndices () == 11);
  assert (foos.numFreeIndices () == 10);

  foos.forEachElement   ([] (Foo&) {});
  foos.forEachFreeIndex ([] (unsigned int) {});

  assert (foos.getSome () == foos.get (0));

  foos.reset ();

  assert (foos.numElements () == 0);
  assert (foos.numIndices () == 0);
  assert (foos.numFreeIndices () == 0);
}

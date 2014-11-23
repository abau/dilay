#include "test-indexable.hpp"
#include "indexable.hpp"

class Foo {
  public:
    Foo (unsigned int, bool) {}
};

void TestIndexable::test () {

  IndexableList <Foo> foos;

  assert (foos.numElements () == 0);
  assert (foos.numIndices () == 0);
  assert (foos.numFreeIndices () == 0);

  foos.emplace (false);

  assert (foos.numElements () == 1);
  assert (foos.numIndices () == 1);
  assert (foos.numFreeIndices () == 0);

  assert (foos.isFree (0) == false);

  foos.reserveIndices (10);

  assert (foos.numElements () == 1);
  assert (foos.numIndices () == 10);
  assert (foos.numFreeIndices () == 9);

  foos.deleteIndex (0);

  assert (foos.numElements () == 0);
  assert (foos.numIndices () == 10);
  assert (foos.numFreeIndices () == 10);

  foos.emplace (true);

  assert (foos.numElements () == 1);
  assert (foos.numIndices () == 10);
  assert (foos.numFreeIndices () == 9);

  foos.forEachElement   ([] (Foo&) {});
  foos.forEachFreeIndex ([] (unsigned int) {});

  assert (foos.getSome () == foos.get (0));

  foos.reset ();

  assert (foos.numElements () == 0);
  assert (foos.numIndices () == 0);
  assert (foos.numFreeIndices () == 0);
}

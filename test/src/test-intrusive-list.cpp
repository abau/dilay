/* This file is part of Dilay
 * Copyright © 2015,2016 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <cassert>
#include "intrusive-list.hpp"
#include "test-intrusive-list.hpp"

namespace {
  class Foo : public IntrusiveList <Foo>::Item {
    public:
      Foo (int d) : _data (d) {}

      int data () { return this->_data; }

    private:
      int _data;
  };
}

void TestIntrusiveList::test1 () {

  IntrusiveList <Foo> list;
  Foo* middle = nullptr;

  assert (list.numElements () == 0);

  for (int i = 1; i <= 10; i++) {
    Foo& f = list.emplaceBack (i);
    if (i == 5) {
      middle = &f;
    }
  }

  assert (list.numElements () == 10);
  assert (list.front ().data () == 1);
  assert (list.back ().data () == 10);
  assert (middle->data () == 5);

  list.popFront ();
  assert (list.numElements () == 9);
  assert (list.front ().data () == 2);
  assert (list.back ().data () == 10);
  assert (middle->data () == 5);


  list.popBack ();
  assert (list.numElements () == 8);
  assert (list.front ().data () == 2);
  assert (list.back ().data () == 9);
  assert (middle->data () == 5);

  list.deleteElement (*middle);
  assert (list.numElements () == 7);
  assert (list.front ().data () == 2);
  assert (list.back ().data () == 9);

  list.reset ();
  assert (list.numElements () == 0);

  list.emplaceBack (42);
  list.emplaceBack (43);
  list.emplaceBack (44);
  assert (list.numElements () == 3);
  assert (list.front ().data () == 42);
  assert (list.back ().data () == 44);

  list.deleteElement (list.front ());
  assert (list.numElements () == 2);
  list.deleteElement (list.back ());
  assert (list.numElements () == 1);
  list.deleteElement (list.front ());
  assert (list.numElements () == 0);
}

namespace {
  class IndexedFoo : public IntrusiveList <IndexedFoo>::Item {
    public:
      IndexedFoo (unsigned int i, int d) : _index (i), _data (d) {}

      unsigned int index () { return this->_index; }
      int          data  () { return this->_data;  }

    private:
      unsigned int _index;
      int          _data;
  };
}

void TestIntrusiveList::test2 () {
  IntrusiveIndexedList <IndexedFoo> list;

  assert (list.numElements () == 0);
  assert (list.hasFreeIndices () == false);

  IndexedFoo& f1 = list.emplaceBack (5);
  list.emplaceBack (10);
  assert (list.numElements () == 2);
  assert (list.hasFreeIndices () == false);
  assert (list.get (0));
  assert (list.get (0)->data () == 5);
  assert (list.get (1));
  assert (list.get (1)->data () == 10);

  list.deleteElement (f1);
  assert (list.numElements () == 1);
  assert (list.hasFreeIndices ());
  assert (list.get (1));
  assert (list.get (1)->data () == 10);
  assert (list.get (1)->index () == 1);
  assert (list.get (0) == nullptr);
  assert (list.isFreeSLOW (0));

  list.emplaceBack (20);
  assert (list.numElements () == 2);
  assert (list.hasFreeIndices () == false);
  assert (list.get (0));
  assert (list.get (0)->data () == 20);
  assert (list.get (0)->index () == 0);

  list.reset ();
  assert (list.numElements () == 0);
  assert (list.hasFreeIndices () == false);
}

void TestIntrusiveList::test3 () {

  IntrusiveList <Foo> list;

  for (int i = 1; i <= 10; i++) {
    list.emplaceBack (i);
  }

  list.deleteElementIf ([] (Foo& f) { return f.data () % 2 == 0; });
  assert (list.numElements () == 5);
  assert (list.front ().data () == 1);
  assert (list.back ().data () == 9);

  IntrusiveList <Foo> list2 (list);
  assert (list2.numElements () == 5);
  assert (list2.front ().data () == 1);
  assert (list2.back ().data () == 9);

  list.deleteElementIf ([] (Foo& f) { return f.data () <= 5; });

  list2 = list;
  assert (list2.numElements () == 2);
  assert (list2.front ().data () == 7);
  assert (list2.back ().data () == 9);

  IntrusiveList <Foo> list3 (std::move (list2));
  assert (list3.numElements () == 2);
  assert (list3.front ().data () == 7);
  assert (list3.back ().data () == 9);
}

/* This file is part of Dilay
 * Copyright © 2015,2016 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <cassert>
#include "monotone-deque.hpp"
#include "test-monotone-deque.hpp"

namespace {
  struct A {
    unsigned int _index;
    unsigned int _value;

    A (unsigned int i, unsigned int v)
      : _index (i)
      , _value (v)
    {}

    A (const A& other)
      : _index (other._index)
      , _value (other._value)
    {}

    unsigned int index () const {
      return this->_index;
    }

    unsigned int value () const {
      return this->_value;
    }
  };

  template <typename T>
  void test (T& v) {
    assert (v.numElements () == 0);
    assert (v.hasFreeIndices () == false);

    A& value1 = v.emplaceElement (5);
    A& value2 = v.emplaceElement (6);
    A& value3 = v.emplaceElement (7);

    assert (v.numElements () == 3);
    assert (v.hasFreeIndices () == false);
    assert (value1.index () == 0);
    assert (value2.index () == 1);
    assert (value3.index () == 2);
    assert (value1.value () == 5);
    assert (value2.value () == 6);
    assert (value3.value () == 7);
    assert (v.elementRef (0).value () == 5);
    assert (v.elementRef (1).value () == 6);
    assert (v.elementRef (2).value () == 7);

    v.deleteElement (value2);

    assert (v.numElements () == 2);
    assert (v.hasFreeIndices ());
    assert (v.element (1) == nullptr);

    A& value2_2 = v.emplaceElement (66);

    assert (v.numElements () == 3);
    assert (v.hasFreeIndices () == false);
    assert (value2_2.index () == 1);
    assert (value2_2.value () == 66);
    assert (v.elementRef (1).value () == 66);

    A& value1_2 = v.insertElement (A (0, 42));

    assert (v.numElements () == 3);
    assert (v.hasFreeIndices () == false);
    assert (value1_2.index () == 0);
    assert (value1_2.value () == 42);
    assert (v.elementRef (0).value () == 42);

    const A foo (2, 69);

    A& value3_2 = v.insertElement (foo);

    assert (v.numElements () == 3);
    assert (v.hasFreeIndices () == false);
    assert (value3_2.index () == 2);
    assert (value3_2.value () == 69);
    assert (v.elementRef (2).value () == 69);
  }
}

void TestMonotoneDeque::test1 () {
  MonotoneDeque <A> v;
  test (v);
}

void TestMonotoneDeque::test2 () {
  MonotonePtrDeque <A> v;
  test (v);
}

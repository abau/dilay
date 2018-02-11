/* This file is part of Dilay
 * Copyright © 2015-2018 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include "maybe.hpp"
#include "test-maybe.hpp"

void TestMaybe::test1 ()
{
  Maybe<int> m1 (5);

  assert (m1.hasValue ());
  assert (*m1 == 5);

  Maybe<int> m2 (m1);

  assert (*m1 == 5);
  assert (*m2 == 5);

  Maybe<int> m3 (std::move (m1));

  assert (m1.hasValue () == false);
  assert (*m3 == 5);

  m2 = 12;
  m3 = std::move (m2);

  assert (m2.hasValue () == false);
  assert (*m3 == 12);

  m3 = std::move (m2);

  assert (m3.hasValue () == false);

  m3 = 12;
  m3 = m2;

  assert (m3.hasValue () == false);

  m3 = 44;
  assert (*m3 == 44);

  int* i = new int(55);
  m3 = i;

  assert (*m3 == 55);

  m3 = nullptr;

  assert (m3.hasValue () == false);
}

namespace
{
  class Foo
  {
  public:
    Foo (int d)
      : _data (d)
    {
    }

    int data () const { return this->_data; }

  private:
    int _data;
  };
}

void TestMaybe::test2 ()
{
  Maybe<Foo> m = Maybe<Foo>::make (5);

  assert (m.hasValue ());
  assert (m->data () == 5);
}

void TestMaybe::test3 ()
{
  Maybe<int> m1 = 5;
  Maybe<int> m2;

  assert (m1.hasValue ());
  assert (m2.hasValue () == false);

  m2 = m1.release ();

  assert (m1.hasValue () == false);
  assert (m2.hasValue ());
  assert (*m2 == 5);
}

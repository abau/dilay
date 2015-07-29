/* This file is part of Dilay
 * Copyright © 2015 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include "maybe.hpp"
#include "test-maybe.hpp"

void TestMaybe::test () {

  Maybe <int> m1 (5);

  assert (m1.isSet ());
  assert (*m1 == 5);

  Maybe <int> m2 (m1);

  assert (*m1 == 5);
  assert (*m2 == 5);

  Maybe <int> m3 (std::move (m1));

  assert (m1.isSet () == false);
  assert (*m3 == 5);

  m2 = 12;
  m3 = std::move (m2);

  assert (m2.isSet () == false);
  assert (*m3 == 12);

  m3 = std::move (m2);

  assert (m3.isSet () == false);

  m3 = 12;
  m3 = m2;

  assert (m3.isSet () == false);

  m3 = 44;
  assert (*m3 == 44);

  int *i = new int (55);
  m3 = i;
  delete i;

  assert (*m3 == 55);

  m3 = nullptr;

  assert (m3.isSet () == false);
}

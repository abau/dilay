#include "maybe.hpp"
#include "test-maybe.hpp"

void TestMaybe::test () {

  Maybe <int> m1;

  assert (m1.isSet () == false);

  m1.set (5);

  assert (m1.isSet ());
  assert (m1.getRef () == 5);

  Maybe <int> m2 (m1);

  assert (m1.getRef () == 5);
  assert (m2.getRef () == 5);

  Maybe <int> m3 (std::move (m1));

  assert (m1.isSet () == false);
  assert (m3.getRef () == 5);

  m2.set (12);
  m3 = std::move (m2);

  assert (m2.isSet () == false);
  assert (m3.getRef () == 12);

  m3 = std::move (m2);

  assert (m3.isSet () == false);

  m3.set (12);
  m3 = m2;

  assert (m3.isSet () == false);
}

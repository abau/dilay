/* This file is part of Dilay
 * Copyright © 2015,2016 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <cassert>
#include "bitset.hpp"
#include "test-bitset.hpp"

void TestBitset::test ()
{
  Bitset<unsigned int> bs;

  assert (bs.get<0> () == false);
  assert (bs.get<8 * sizeof (unsigned int) - 1> () == false);

  bs.toggle<0> ();
  bs.toggle<8 * sizeof (unsigned int) - 1> ();
  // bs.toggle <8 * sizeof (unsigned int)> ();      // must not compile

  assert (bs.get<0> ());
  assert (bs.get<1> () == false);
  assert (bs.get<8 * sizeof (unsigned int) - 1> ());
  assert (bs.value () == std::pow (2, 8 * sizeof (unsigned int) - 1) + 1);

  bs.reset ();
  assert (bs.none ());

  bs.set<0> ();
  bs.set<1> ();
  bs.set<2> ();
  assert (bs.none () == false);
  assert (bs.all<2> ());
  assert (bs.all<3> ());
  assert (bs.all<4> () == false);
  assert (bs.value () == 7);
}

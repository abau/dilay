/* This file is part of Dilay
 * Copyright © 2015 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <cassert>
#include "bitset.hpp"
#include "test-bitset.hpp"

void TestBitset::test () {

  Bitset <int> bs;

  assert (bs.get <0> () == false);
  assert (bs.get <8 * sizeof (int) - 1> () == false);

  bs.toggle <0> ();
  bs.toggle <8 * sizeof (int) - 1> ();
  // bs.toggle <8 * sizeof (int)> ();      // must not compile

  assert (bs.get <0> ());
  assert (bs.get <8 * sizeof (int) - 1> ());
}

#include "bitset.hpp"
#include "test-bitset.hpp"

void TestBitset::test () {

  Bitset <int> bs;

  assert (bs.get (0) == false);
  assert (bs.get (8 * sizeof (int) - 1) == false);

  bs.toggle (0);
  bs.toggle (8 * sizeof (int) - 1);

  assert (bs.get (0));
  assert (bs.get (8 * sizeof (int) - 1));
}

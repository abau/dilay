/* This file is part of Dilay
 * Copyright © 2015-2017 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include "test-prune.hpp"
#include "util.hpp"

namespace
{
  bool equals (std::vector<int> prune, const std::vector<int>& vec,
               const std::vector<unsigned int>& indices)
  {
    std::vector<unsigned int> pruneIndices;

    Util::prune<int> (prune, [](int i) { return i < 0; }, &pruneIndices);

    return std::equal (prune.begin (), prune.end (), vec.begin (), vec.end ()) &&
           std::equal (pruneIndices.begin (), pruneIndices.end (), indices.begin (),
                       indices.end ());
  }
}

void TestPrune::test ()
{
  const unsigned int x = Util::invalidIndex ();

  assert (
    equals ({1, 2, 3, 4, 5, 6, 7, 8, 9}, {1, 2, 3, 4, 5, 6, 7, 8, 9}, {0, 1, 2, 3, 4, 5, 6, 7, 8}));
  assert (equals ({-1, 2, -3, 4, -5, 6, -7, 8, -9}, {8, 2, 6, 4}, {x, 1, x, 3, x, 2, x, 0, x}));
  assert (equals ({-1, -2, -3, -4, -5, -6, -7, -8, -9}, {}, {x, x, x, x, x, x, x, x, x}));
  assert (equals ({-1, -2, -3, -4, -5, -6, -7, -8, 9}, {9}, {x, x, x, x, x, x, x, x, 0}));
  assert (equals ({1, -2, -3, -4, -5, -6, -7, -8, 9}, {1, 9}, {0, x, x, x, x, x, x, x, 1}));
  assert (equals ({-1, 2, -3, -4, -5, -6, -7, -8, 9}, {9, 2}, {x, 1, x, x, x, x, x, x, 0}));
  assert (equals ({-1, -2, -3, -4, -5, -6, 7, 8, 9}, {9, 8, 7}, {x, x, x, x, x, x, 2, 1, 0}));
}

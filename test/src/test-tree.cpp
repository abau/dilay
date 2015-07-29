/* This file is part of Dilay
 * Copyright © 2015 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <cassert>
#include "tree.hpp"
#include "test-tree.hpp"

void TestTree::test () {

  Tree <int> t (0);

  Tree <int>& n1 = t.emplaceChild (100);
  Tree <int>& n2 = n1.emplaceChild (200);

  assert (n1.data () == 100);
  assert (n2.parent () == &n1);
  assert (t.numElements () == 3);

  t.deleteChild (n1);
  assert (t.numElements () == 1);
}

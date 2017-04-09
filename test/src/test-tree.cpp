/* This file is part of Dilay
 * Copyright © 2015,2016 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <cassert>
#include "test-tree.hpp"
#include "tree.hpp"

namespace
{
  struct Foo
  {
    int i;

    Foo (int v)
      : i (v)
    {
    }
  };
}

void TestTree::test1 ()
{
  Tree<Foo> tree;

  auto& n0 = tree.emplaceRoot (0);
  auto& n1 = n0.emplaceChild (100);
  auto& n2 = n1.emplaceChild (200);

  assert (n1.data ().i == 100);
  assert (n2.parent () == &n1);
  assert (tree.root ().numNodes () == 3);
  assert (n1.lastChild ().data ().i == n2.data ().i);

  n0.deleteChild (n1);
  assert (tree.root ().numNodes () == 1);

  n0.emplaceChild (5);
  n0.emplaceChild (6).emplaceChild (7);

  assert (tree.root ().numNodes () == 4);

  n0.deleteChildIf ([](const auto& c) { return c.data ().i == 6; });

  assert (tree.root ().numNodes () == 2);
  assert (n0.lastChild ().data ().i == 5);

  Tree<Foo> copy (tree);

  tree.reset ();

  assert (copy.root ().numNodes () == 2);
  assert (copy.root ().data ().i == 0);
  assert (copy.root ().lastChild ().data ().i == 5);
  assert (copy.root ().lastChild ().parent () == &copy.root ());
}

void TestTree::test2 ()
{
  Tree<int> t;

  t.emplaceRoot (1).emplaceChild (2).emplaceChild (3).emplaceChild (4);

  assert (t.root ().numNodes () == 4);
  assert (t.root ().data () == 1);
  assert (t.root ().lastChild ().data () == 2);
  assert (t.root ().lastChild ().lastChild ().data () == 3);
  assert (t.root ().lastChild ().lastChild ().lastChild ().data () == 4);

  t.rebalance (t.root ().lastChild ().lastChild ().lastChild ());

  assert (t.root ().numNodes () == 4);
  assert (t.root ().data () == 4);
  assert (t.root ().lastChild ().data () == 3);
  assert (t.root ().lastChild ().lastChild ().data () == 2);
  assert (t.root ().lastChild ().lastChild ().lastChild ().data () == 1);
}

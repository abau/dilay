/* This file is part of Dilay
 * Copyright © 2015 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <cassert>
#include "tree.hpp"
#include "test-tree.hpp"

namespace {
  struct Foo {
    int i;
    TreeNode <Foo>* parent;

    Foo (int v, TreeNode <Foo>* p = nullptr)
      : i (v)
      , parent (p)
    {}

    void updatePointers (const TreeNodePtrMap <Foo>& ptrMap) {
      this->parent = ptrMap.at (this->parent);
    }
  };
}

void TestTree::test () {

  Tree <Foo> tree;

  auto& n0 = tree.emplaceRoot (0);
  auto& n1 = n0.emplaceChild (100, &n0);
  auto& n2 = n1.emplaceChild (200, &n1);

  assert (n1.data ().i == 100);
  assert (n2.data ().parent == &n1);
  assert (tree.root ().numNodes () == 3);
  assert (n1.lastChild ().data ().i == n2.data ().i);

  n0.deleteChild (n1);
  assert (tree.root ().numNodes () == 1);

  n0.emplaceChild (5, &n0);
  n0.emplaceChild (6, &n0).emplaceChild (7);

  assert (tree.root ().numNodes () == 4);

  n0.deleteChildIf ([] (const auto& c) { return c.data ().i == 6; });

  assert (tree.root ().numNodes () == 2);
  assert (n0.lastChild ().data ().i == 5);

  Tree <Foo> copy (tree);

  tree.reset ();
  
  assert (copy.root ().numNodes () == 2);
  assert (copy.root ().data ().i == 0);
  assert (copy.root ().lastChild ().data ().i == 5);
  assert (copy.root ().lastChild ().data ().parent == &copy.root ());
}

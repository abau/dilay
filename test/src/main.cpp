/* This file is part of Dilay
 * Copyright © 2015-2018 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <QCoreApplication>
#include <iostream>
#include "test-bitset.hpp"
#include "test-distance.hpp"
#include "test-intersection.hpp"
#include "test-maybe.hpp"
#include "test-misc.hpp"
#include "test-octree.hpp"
#include "test-prune.hpp"
#include "test-tree.hpp"

int main ()
{
  QCoreApplication::setApplicationName ("dilay");

  TestIntersection::test1 ();
  TestIntersection::test2 ();
  TestMaybe::test1 ();
  TestMaybe::test2 ();
  TestMaybe::test3 ();
  TestOctree::test ();
  TestBitset::test ();
  TestTree::test1 ();
  TestTree::test2 ();
  TestTree::test3 ();
  TestMisc::test ();
  TestDistance::test ();
  TestPrune::test ();

  std::cout << "all tests ran successfully\n";
  return 0;
}

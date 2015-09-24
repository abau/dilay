/* This file is part of Dilay
 * Copyright © 2015 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <iostream>
#include <QCoreApplication>
#include "test-bitset.hpp"
#include "test-intersection.hpp"
#include "test-intrusive-list.hpp"
#include "test-maybe.hpp"
#include "test-misc.hpp"
#include "test-octree.hpp"
#include "test-tree.hpp"

int main () {
  QCoreApplication::setApplicationName ("dilay");

  TestIntersection ::test  ();
  TestMaybe        ::test1 ();
  TestMaybe        ::test2 ();
  TestMaybe        ::test3 ();
  TestOctree       ::test1 ();
  TestOctree       ::test2 ();
  TestBitset       ::test  ();
  TestIntrusiveList::test1 ();
  TestIntrusiveList::test2 ();
  TestIntrusiveList::test3 ();
  TestTree         ::test1 ();
  TestTree         ::test2 ();
  TestMisc         ::test  ();

  std::cout << "all tests run successfully\n";
  return 0;
}


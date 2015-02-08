#include <iostream>
#include <QCoreApplication>
#include "test-bitset.hpp"
#include "test-indexable.hpp"
#include "test-intersection.hpp"
#include "test-maybe.hpp"
#include "test-octree.hpp"

int main () {
  QCoreApplication::setApplicationName ("dilay");

  TestIndexable   ::test  ();
  TestIntersection::test  ();
  TestMaybe       ::test  ();
  TestOctree      ::test1 ();
  TestOctree      ::test2 ();
  TestBitset      ::test  ();

  std::cout << "all tests run successfully\n";
  return 0;
}


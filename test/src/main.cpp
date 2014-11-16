#include <iostream>
#include <QCoreApplication>
#include "test-indexable.hpp"
#include "test-intersection.hpp"
#include "test-octree.hpp"

int main () {
  QCoreApplication::setApplicationName ("dilay");

  TestIndexable   ::test  ();
  TestIntersection::test  ();
  TestOctree      ::test1 ();
  TestOctree      ::test2 ();

  std::cout << "all tests run successfully\n";
  return 0;
}


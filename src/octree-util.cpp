#include <iostream>
#include "octree.hpp"
#include "octree-util.hpp"

void OctreeUtil :: printStatistics (const Octree& octree) {
  unsigned int numNodes = 0;

  for (auto it = octree.nodeIterator (); it.hasNode (); it.next ()) {
    numNodes++;
  }

  std::cout << "Octree:\n"
            << "\tnum nodes:\t" << numNodes
            << std::endl;
}

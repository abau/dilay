#include <iostream>
#include <limits>
#include <unordered_map>
#include <glm/glm.hpp>
#include "octree.hpp"
#include "octree-util.hpp"
#include "util.hpp"

unsigned int OctreeUtil :: numFaces (const Octree& octree) {
  unsigned int i = 0;
  for (auto it = octree.nodeIterator (); it.isValid (); it.next ()) {
    i = i + it.element ().numFaces ();
  }
  return i;
}

void OctreeUtil :: printStatistics (const Octree& octree) {
  unsigned int numNodes      = 0;
  unsigned int maxFaces      = 0;
  unsigned int numFacesCheck = 0;
           int minDepth      = std::numeric_limits <int> :: max ();
           int maxDepth      = std::numeric_limits <int> :: min ();

  typedef std::unordered_map <int, unsigned int> DepthMap;
  DepthMap depthFacesMap;
  DepthMap depthNodesMap;

  for (auto it = octree.nodeIterator (); it.isValid (); it.next ()) {
    numNodes++;
    const OctreeNode& node = it.element ();
    maxFaces = std::max <unsigned int> (maxFaces, node.numFaces ());
    minDepth = std::min <int> (minDepth, node.depth ());
    maxDepth = std::max <int> (maxDepth, node.depth ());

    DepthMap::iterator e = depthFacesMap.find (node.depth ());
    if (e == depthFacesMap.end ())
      depthFacesMap.emplace (node.depth (), node.numFaces ());
    else
      e->second += node.numFaces ();

    e = depthNodesMap.find (node.depth ());
    if (e == depthNodesMap.end ())
      depthNodesMap.emplace (node.depth (), 1);
    else
      e->second ++;
  }

  for (auto it = octree.faceIterator (); it.isValid (); it.next ()) {
    numFacesCheck++;
  }

  std::cout << "Octree:"
            << "\n\tnum nodes:\t\t" << numNodes
            << "\n\tnum faces:\t\t" << OctreeUtil :: numFaces (octree) 
            << " (" << numFacesCheck << ")"
            << "\n\tmax faces per node:\t" << maxFaces
            << "\n\tmin depth:\t\t" << minDepth
            << "\n\tmax depth:\t\t" << maxDepth
            << std::endl;

  if (numNodes <= 20) {
    for (auto it = octree.nodeIterator (); it.isValid (); it.next ()) {
      const OctreeNode& node = it.element ();
      std::cout << "\tOctree node:"
                << "\n\t\tcenter:\t\t"     << node.center ()
                << "\n\t\twidth:\t\t"      << node.width ()
                << "\n\t\tdepth:\t\t"      << node.depth ()
                << "\n\t\tnum faces:\t"    << node.numFaces ()
                << std::endl;
    }
  }

  for (DepthMap::value_type& e : depthNodesMap) {
    std::cout << "\t" << e.second << "\tnodes at depth\t" << e.first << std::endl;
  }
  for (DepthMap::value_type& e : depthFacesMap) {
    std::cout << "\t" << e.second << "\tfaces at depth\t" << e.first << std::endl;
  }
}

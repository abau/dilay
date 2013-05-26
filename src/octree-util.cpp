#include <iostream>
#include <limits>
#include <unordered_map>
#include <glm/glm.hpp>
#include "octree.hpp"
#include "octree-util.hpp"
#include "util.hpp"

unsigned int OctreeUtil :: numFaces (const Octree& octree) {
  unsigned int i = 0;
  for (auto it = octree.nodeIterator (); it.hasNode (); it.next ()) {
    i = i + it.node ().numFaces ();
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

  for (auto it = octree.nodeIterator (); it.hasNode (); it.next ()) {
    numNodes++;
    maxFaces = std::max <unsigned int> (maxFaces, it.node ().numFaces ());
    minDepth = std::min <int> (minDepth, it.node ().depth ());
    maxDepth = std::max <int> (maxDepth, it.node ().depth ());

    DepthMap::iterator e = depthFacesMap.find (it.node ().depth ());
    if (e == depthFacesMap.end ())
      depthFacesMap.emplace (it.node ().depth (), it.node ().numFaces ());
    else
      e->second += it.node ().numFaces ();

    e = depthNodesMap.find (it.node ().depth ());
    if (e == depthNodesMap.end ())
      depthNodesMap.emplace (it.node ().depth (), 1);
    else
      e->second ++;
  }

  for (auto it = octree.faceIterator (); it.hasFace (); it.next ()) {
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
    for (auto it = octree.nodeIterator (); it.hasNode (); it.next ()) {
      std::cout << "\tOctree node:"
                << "\n\t\tcenter:\t\t"     << it.node ().center ()
                << "\n\t\twidth:\t\t"      << it.node ().width ()
                << "\n\t\tdepth:\t\t"      << it.node ().depth ()
                << "\n\t\tnum faces:\t"    << it.node ().numFaces ()
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

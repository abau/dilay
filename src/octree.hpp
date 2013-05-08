#ifndef DILAY_OCTREE
#define DILAY_OCTREE

#include "fwd-winged.hpp"

class Triangle;
class OctreeImpl;

class Octree {
  public:
          Octree            ();
          Octree            (const Octree&);
    const Octree& operator= (const Octree&);
         ~Octree            ();

    LinkedFace insertFace (const WingedFace&, const Triangle&);
    void render ();

  private:
    OctreeImpl* impl;
};
#endif

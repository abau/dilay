#ifndef DILAY_OCTREE
#define DILAY_OCTREE

#include "fwd-winged.hpp"

class Triangle;
class OctreeImpl;
class OctreeIteratorImpl;

class Octree { friend class OctreeIteratorImpl;
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

/** Iterates over all faces of an octree */
class OctreeIterator {
  public:        OctreeIterator (Octree&);

    bool         hasFace        () const;
    LinkedFace   face           () const;
    void         next           ();
    unsigned int depth          () const;

  private:
    OctreeIteratorImpl* impl;
};

#endif

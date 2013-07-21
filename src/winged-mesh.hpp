#ifndef DILAY_WINGED_MESH
#define DILAY_WINGED_MESH

#include "fwd-winged.hpp"
#include "fwd-glm.hpp"

class FaceIntersection;
class Ray;
class Triangle;
class Octree;
class OctreeFaceIterator;
class ConstOctreeFaceIterator;
class OctreeNodeIterator;
class ConstOctreeNodeIterator;

class WingedMesh {
  public:                    
          WingedMesh                  ();
          WingedMesh                  (const WingedMesh&);
    const WingedMesh& operator=       (const WingedMesh&);
         ~WingedMesh                  ();

    void              addIndex        (unsigned int);
    LinkedVertex      addVertex       (const glm::vec3&, unsigned int = 0);
    LinkedEdge        addEdge         (const WingedEdge&);
    LinkedFace        addFace         (const WingedFace&, const Triangle&);

    const Vertices&   vertices        () const;
    const Edges&      edges           () const;
    const Octree&     octree          () const;

    OctreeFaceIterator      octreeFaceIterator ();
    ConstOctreeFaceIterator octreeFaceIterator () const;
    OctreeNodeIterator      octreeNodeIterator ();
    ConstOctreeNodeIterator octreeNodeIterator () const;

    /** Deletes an edge and its _right_ face. Note that other parts of the program
     * depend on this behaviour. */
    LinkedFace      deleteEdge        (LinkedEdge);

    /** Realigns a face in a mesh's octree. The passed `LinkedFace` becomes invalid
     * and must not be dereferenced: use the returned `LinkedFace` instead. */
    LinkedFace      realignInOctree   (LinkedFace);

    unsigned int    numVertices       () const;
    unsigned int    numWingedVertices () const;
    unsigned int    numEdges          () const;
    unsigned int    numFaces          () const;

    glm::vec3       vertex            (unsigned int) const;

    void            rebuildIndices    ();
    void            rebuildNormals    ();
    void            bufferData        ();
    void            render            ();
    void            reset             ();
    void            reset             (const glm::vec3&, float);
    void            toggleRenderMode  ();
    
    void            intersectRay      (const Ray&, FaceIntersection&);
  private:
    class Impl;
    Impl* impl;
};

#endif

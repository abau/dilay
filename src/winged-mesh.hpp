#ifndef DILAY_WINGED_MESH
#define DILAY_WINGED_MESH

#include <list>
#include "fwd-glm.hpp"
#include "fwd-winged.hpp"

class FaceIntersection;
class Ray;
class Triangle;
class Octree;
class OctreeFaceIterator;
class ConstOctreeFaceIterator;
class OctreeNodeIterator;
class ConstOctreeNodeIterator;
class WingedVertex;
class WingedFace;
class WingedEdge;
class Id;

class WingedMesh {
  public: WingedMesh                  ();
          WingedMesh                  (const WingedMesh&);
    const WingedMesh& operator=       (const WingedMesh&);
         ~WingedMesh                  ();

    unsigned int      addIndex        (unsigned int);
    WingedVertex&     addVertex       (const glm::vec3&, unsigned int = 0);
    WingedEdge&       addEdge         (const WingedEdge&);
    WingedFace&       addFace         (const WingedFace&, const Triangle&);
    void              setIndex        (unsigned int, unsigned int);
    void              setVertex       (unsigned int, const glm::vec3&);
    void              setNormal       (unsigned int, const glm::vec3&);

    const Vertices&   vertices () const;
    const Edges&      edges    () const;
    const Octree&     octree   () const;

    OctreeFaceIterator      octreeFaceIterator ();
    ConstOctreeFaceIterator octreeFaceIterator () const;
    OctreeNodeIterator      octreeNodeIterator ();
    ConstOctreeNodeIterator octreeNodeIterator () const;

    void            releaseFirstIndexNumber (WingedFace&);
    /** Deletes an edge and its _right_ face. Note that other parts of the program
     * depend on this behaviour. */
    WingedFace&     deleteEdge        (WingedEdge&);

    /** Realigns a face in a mesh's octree. The passed face becomes invalid
     * and must not be used: use the returned face instead. */
    WingedFace&     realignInOctree   (WingedFace&);

    unsigned int    numVertices       () const;
    unsigned int    numWingedVertices () const;
    unsigned int    numEdges          () const;
    unsigned int    numFaces          () const;
    unsigned int    numIndices        () const;

    glm::vec3       vertex            (unsigned int) const;

    void            write             (); 
    void            bufferData        ();
    void            render            ();
    void            reset             ();
    void            reset             (const glm::vec3&, float);
    void            toggleRenderMode  ();
    
    void            intersectRay      (const Ray&, FaceIntersection&);

    bool            hasFreeFirstIndexNumber  () const;
    unsigned int    nextFreeFirstIndexNumber ();
  private:
    class Impl;
    Impl* impl;
};

#endif

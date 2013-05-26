#ifndef DILAY_WINGED_MESH
#define DILAY_WINGED_MESH

#include "fwd-winged.hpp"
#include "fwd-glm.hpp"

class FaceIntersection;
class Ray;
class Triangle;
class Octree;

class WingedMeshImpl;

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

    /** Deletes an edge and its _right_ face. Note that other parts of the program
     * depend on this behaviour. */
    LinkedFace      deleteEdge        (LinkedEdge);

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
    void            toggleRenderMode  ();
    
    void            intersectRay      (const Ray&, FaceIntersection&);
  private:
    WingedMeshImpl* impl;
};

#endif

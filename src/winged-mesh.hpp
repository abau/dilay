#include <glm/glm.hpp>
#include "mesh.hpp"
#include "winged-edge.hpp"
#include "winged-face.hpp"
#include "winged-vertex.hpp"
#include "linked-list.hpp"
#include "maybe.hpp"
#include "ray.hpp"
#include "intersection.hpp"

#ifndef WINGED_MESH
#define WINGED_MESH

typedef LinkedList < WingedVertex > Vertices;
typedef LinkedList < WingedEdge   > Edges;
typedef LinkedList < WingedFace   > Faces;

typedef Vertices::Iterator VertexIterator;
typedef Edges   ::Iterator EdgeIterator;
typedef Faces   ::Iterator FaceIterator;

typedef Vertices::ConstIterator VertexConstIterator;
typedef Edges   ::ConstIterator EdgeConstIterator;
typedef Faces   ::ConstIterator FaceConstIterator;

class WingedMesh {
  private:
    Mesh                     mesh;
    Vertices                 vertices;
    Edges                    edges;
    Faces                    faces;

    void                     addIndices         ();

  public:
    void                     addIndex       (unsigned int);
    LinkedVertex*            addVertex      (const glm::vec3&, LinkedEdge*);
    LinkedEdge*              addEdge        (const WingedEdge&);
    LinkedFace*              addFace        (const WingedFace&);

    VertexIterator           vertexIterator ();
    EdgeIterator             edgeIterator   ();
    FaceIterator             faceIterator   ();

    VertexConstIterator      vertexIterator () const;
    EdgeConstIterator        edgeIterator   () const;
    FaceConstIterator        faceIterator   () const;

    unsigned int             numVertices    () const;
    unsigned int             numEdges       () const;
    unsigned int             numFaces       () const;

    glm::vec3                vertex         (unsigned int) const;

    void                     rebuildIndices ();
    void                     bufferData     ();
    void                     render         ();
    void                     printStats     () const;
    
    Maybe <FaceIntersection> intersectRay   (const Ray&);

    static WingedMesh* triangle (const glm::vec3&, const glm::vec3&, const glm::vec3&);
};

#endif

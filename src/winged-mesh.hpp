#ifndef DILAY_WINGED_MESH
#define DILAY_WINGED_MESH

#include <list>
#include "fwd-winged.hpp"
#include "fwd-glm.hpp"

typedef std::list < WingedVertex > Vertices;
typedef std::list < WingedEdge   > Edges;
typedef std::list < WingedFace   > Faces;

typedef Vertices::iterator VertexIterator;
typedef Edges   ::iterator EdgeIterator;
typedef Faces   ::iterator FaceIterator;

typedef Vertices::const_iterator VertexConstIterator;
typedef Edges   ::const_iterator EdgeConstIterator;
typedef Faces   ::const_iterator FaceConstIterator;

#define VERTEX_ITERATOR(it,mesh)  VertexIterator (it) = (mesh).vertexIterator (); (it) != (mesh).nullVertex (); ++(it)
#define EDGE_ITERATOR(it,mesh)  EdgeIterator (it) = (mesh).edgeIterator (); (it) != (mesh).nullEdge (); ++(it)
#define FACE_ITERATOR(it,mesh)  FaceIterator (it) = (mesh).faceIterator (); (it) != (mesh).nullFace (); ++(it)

#define VERTEX_CONST_ITERATOR(it,mesh)  VertexConstIterator (it) = (mesh).vertexIterator (); (it) != (mesh).nullVertex (); ++(it)
#define EDGE_CONST_ITERATOR(it,mesh)  EdgeConstIterator (it) = (mesh).edgeIterator (); (it) != (mesh).nullEdge (); ++(it)
#define FACE_CONST_ITERATOR(it,mesh)  FaceConstIterator (it) = (mesh).faceIterator (); (it) != (mesh).nullFace (); ++(it)

#define VERTEX_REVERSE_ITERATOR(it,mesh)  VertexIterator (it) = (mesh).vertexReverseIterator (); (it) != (mesh).rNullVertex (); --(it)
#define EDGE_REVERSE_ITERATOR(it,mesh)  EdgeIterator (it) = (mesh).edgeReverseIterator (); (it) != (mesh).rNullEdge (); --(it)
#define FACE_REVERSE_ITERATOR(it,mesh)  FaceIterator (it) = (mesh).faceReverseIterator (); (it) != (mesh).rNullFace (); --(it)

#define VERTEX_CONST_REVERSE_ITERATOR(it,mesh)  VertexConstIterator (it) = (mesh).vertexReverseIterator (); (it) != (mesh).rNullVertex (); --(it)
#define EDGE_CONST_REVERSE_ITERATOR(it,mesh)  EdgeConstIterator (it) = (mesh).edgeReverseIterator (); (it) != (mesh).rNullEdge (); --(it)
#define FACE_CONST_REVERSE_ITERATOR(it,mesh)  FaceConstIterator (it) = (mesh).faceReverseIterator (); (it) != (mesh).rNullFace (); --(it)

class FaceIntersection;
class Ray;

class WingedMeshImpl;

class WingedMesh {
  public:                    
     WingedMesh                                    ();
     WingedMesh                                    (const WingedMesh&);
     WingedMesh&             operator=             (const WingedMesh&);
    ~WingedMesh                                    ();

    LinkedVertex             nullVertex            ();
    LinkedEdge               nullEdge              ();
    LinkedFace               nullFace              ();
    ConstLinkedVertex        nullVertex            () const;
    ConstLinkedEdge          nullEdge              () const;
    ConstLinkedFace          nullFace              () const;

    LinkedVertex             rNullVertex           ();
    LinkedEdge               rNullEdge             ();
    LinkedFace               rNullFace             ();
    ConstLinkedVertex        rNullVertex           () const;
    ConstLinkedEdge          rNullEdge             () const;
    ConstLinkedFace          rNullFace             () const;

    void                     addIndex              (unsigned int);
    LinkedVertex             addVertex             (const glm::vec3&, LinkedEdge);
    LinkedEdge               addEdge               (const WingedEdge&);
    LinkedFace               addFace               (const WingedFace&);

    VertexIterator           vertexIterator        ();
    EdgeIterator             edgeIterator          ();
    FaceIterator             faceIterator          ();

    VertexConstIterator      vertexIterator        () const;
    EdgeConstIterator        edgeIterator          () const;
    FaceConstIterator        faceIterator          () const;

    VertexIterator           vertexReverseIterator ();
    EdgeIterator             edgeReverseIterator   ();
    FaceIterator             faceReverseIterator   ();

    VertexConstIterator      vertexReverseIterator () const;
    EdgeConstIterator        edgeReverseIterator   () const;
    FaceConstIterator        faceReverseIterator   () const;

    unsigned int             numVertices           () const;
    unsigned int             numWingedVertices     () const;
    unsigned int             numEdges              () const;
    unsigned int             numFaces              () const;

    glm::vec3                vertex                (unsigned int) const;

    void                     rebuildIndices        ();
    void                     rebuildNormals        ();
    void                     bufferData            ();
    void                     renderBegin           ();
    void                     render                ();
    void                     renderEnd             ();
    void                     reset                 ();
    void                     toggleRenderMode      ();
    
    bool                     intersectRay          (const Ray&, FaceIntersection&);
  private:
    WingedMeshImpl* impl;
};

#endif

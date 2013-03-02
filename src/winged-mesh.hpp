#ifndef WINGED_MESH
#define WINGED_MESH

#include <glm/glm.hpp>
#include <list>
#include "mesh.hpp"
#include "winged-edge.hpp"
#include "winged-face.hpp"
#include "winged-vertex.hpp"
#include "maybe.hpp"
#include "ray.hpp"
#include "intersection.hpp"

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

class WingedMesh {
  public:                    
    WingedMesh&              operator=         (const WingedMesh&) = delete;
    LinkedVertex             nullVertex        ()       { return this->vertices.end (); }
    LinkedEdge               nullEdge          ()       { return this->edges.end    (); }
    LinkedFace               nullFace          ()       { return this->faces.end    (); }
    ConstLinkedVertex        nullVertex        () const { return this->vertices.end (); }
    ConstLinkedEdge          nullEdge          () const { return this->edges.end    (); }
    ConstLinkedFace          nullFace          () const { return this->faces.end    (); }

    void                     addIndex          (unsigned int);
    LinkedVertex             addVertex         (const glm::vec3&, LinkedEdge);
    LinkedEdge               addEdge           (const WingedEdge&);
    LinkedFace               addFace           (const WingedFace&);

    VertexIterator           vertexIterator    ();
    EdgeIterator             edgeIterator      ();
    FaceIterator             faceIterator      ();

    VertexConstIterator      vertexIterator    () const;
    EdgeConstIterator        edgeIterator      () const;
    FaceConstIterator        faceIterator      () const;

    unsigned int             numVertices       () const;
    unsigned int             numWingedVertices () const;
    unsigned int             numEdges          () const;
    unsigned int             numFaces          () const;

    glm::vec3                vertex            (unsigned int) const;

    void                     rebuildIndices    ();
    void                     rebuildNormals    ();
    void                     bufferData        () { this->mesh.bufferData (); }
    void                     renderBegin       () { this->mesh.renderBegin (); }
    void                     render            () { this->mesh.render (); }
    void                     renderEnd         () { this->mesh.renderEnd   (); }
    void                     reset             ();
    void                     toggleRenderMode  () { this->mesh.toggleRenderMode (); }
    
    Maybe <FaceIntersection> intersectRay      (const Ray&);
  private:
    Mesh                     mesh;
    Vertices                 vertices;
    Edges                    edges;
    Faces                    faces;
};

#endif

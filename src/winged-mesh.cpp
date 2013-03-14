#include <glm/glm.hpp>
#include "winged-vertex.hpp"
#include "winged-edge.hpp"
#include "winged-face.hpp"
#include "winged-mesh.hpp"
#include "winged-mesh-util.hpp"
#include "mesh.hpp"
#include "intersection.hpp"
#include "triangle.hpp"
#include "ray.hpp"
#include "macro.hpp"

struct WingedMeshImpl {
  WingedMesh* wingedMesh;
  Mesh        mesh;
  Vertices    vertices;
  Edges       edges;
  Faces       faces;

  WingedMeshImpl (WingedMesh* p) : wingedMesh (p) {}

  LinkedVertex      nullVertex  ()       { return this->vertices.end (); }
  LinkedEdge        nullEdge    ()       { return this->edges.end    (); }
  LinkedFace        nullFace    ()       { return this->faces.end    (); }
  ConstLinkedVertex nullVertex  () const { return this->vertices.end (); }
  ConstLinkedEdge   nullEdge    () const { return this->edges.end    (); }
  ConstLinkedFace   nullFace    () const { return this->faces.end    (); }

  LinkedVertex      rNullVertex ()       { return --this->vertices.begin (); }
  LinkedEdge        rNullEdge   ()       { return --this->edges.begin    (); }
  LinkedFace        rNullFace   ()       { return --this->faces.begin    (); }
  ConstLinkedVertex rNullVertex () const { return --this->vertices.begin (); }
  ConstLinkedEdge   rNullEdge   () const { return --this->edges.begin    (); }
  ConstLinkedFace   rNullFace   () const { return --this->faces.begin    (); }

  void addIndex (unsigned int index)     { this->mesh.addIndex (index); }

  LinkedVertex addVertex (const glm::vec3& v, LinkedEdge e) {
    unsigned int index = this->mesh.addVertex (v);
    this->vertices.push_back (WingedVertex (index, e));
    return --this->vertices.end ();
  }

  LinkedEdge addEdge (const WingedEdge& e) {
    this->edges.push_back (e);
    return --this->edges.end ();
  }

  LinkedFace addFace (const WingedFace& f) {
    this->faces.push_back (f);
    return --this->faces.end ();
  }

  VertexIterator vertexIterator () { 
    return this->vertices.begin ();
  }
  EdgeIterator edgeIterator () {
    return this->edges.begin ();
  }
  FaceIterator faceIterator () {
    return this->faces.begin ();
  }

  VertexConstIterator vertexIterator () const { 
    return this->vertices.cbegin ();
  }
  EdgeConstIterator edgeIterator () const {
    return this->edges.cbegin ();
  }
  FaceConstIterator faceIterator () const {
    return this->faces.cbegin ();
  }

  VertexIterator vertexReverseIterator () { return --this->vertices.end (); }
  EdgeIterator edgeReverseIterator ()     { return --this->edges.end (); }
  FaceIterator faceReverseIterator ()     { return --this->faces.end (); }

  VertexConstIterator vertexReverseIterator () const { 
    return --this->vertices.cend ();
  }
  EdgeConstIterator edgeReverseIterator () const { return --this->edges.cend (); }
  FaceConstIterator faceReverseIterator () const { return --this->faces.cend (); }

  unsigned int numVertices       () const { return this->mesh.numVertices (); }
  unsigned int numWingedVertices () const { return this->vertices.size    (); }
  unsigned int numEdges          () const { return this->edges.size       (); }
  unsigned int numFaces          () const { return this->faces.size       (); }

  glm::vec3 vertex (unsigned int i) const { return this->mesh.vertex (i); }

  void rebuildIndices () {
    this->mesh.clearIndices ();
    for (WingedFace& f : this->faces) {
      f.addIndices (*this->wingedMesh);
    }
  }

  void rebuildNormals () {
    this->mesh.clearNormals ();
    for (WingedVertex& v : this->vertices) {
      this->mesh.addNormal (v.normal (*this->wingedMesh));
    }
  }

  void bufferData  () { this->mesh.bufferData   (); }
  void renderBegin () { this->mesh.renderBegin  (); }
  void render      () { this->mesh.render       (); }
  void renderEnd   () { this->mesh.renderEnd    (); }

  void reset () {
    this->mesh    .reset ();
    this->vertices.clear ();
    this->edges   .clear ();
    this->faces   .clear ();
  }

  void toggleRenderMode () { this->mesh.toggleRenderMode (); }

  bool intersectRay (const Ray& ray, FaceIntersection& intersection) {
    bool      isIntersection = false;
    float     minDistance    = 0.0f;
    Triangle  triangle;
    glm::vec3 p;

    for (FACE_ITERATOR(fIt,*this)) {
      fIt->triangle (*this->wingedMesh,triangle);
      if (triangle.intersectRay (ray,p) == false) {
        continue;
      }
      else {
        float d = glm::distance (ray.origin (), p);

        if (isIntersection == false || d < minDistance) {
          isIntersection        = true;
          minDistance           = d;
          intersection.position (p);
          intersection.face     (fIt);
        }
      }
    }
    return isIntersection;
  }
};

WingedMesh :: WingedMesh () : impl (new WingedMeshImpl (this)) {}

DELEGATE_COPY_CONSTRUCTOR (WingedMesh)
DELEGATE_ASSIGNMENT_OP    (WingedMesh)
DELEGATE_DESTRUCTOR       (WingedMesh)

DELEGATE        (LinkedVertex        , WingedMesh, nullVertex)
DELEGATE        (LinkedEdge          , WingedMesh, nullEdge)
DELEGATE        (LinkedFace          , WingedMesh, nullFace)
DELEGATE_CONST  (ConstLinkedVertex   , WingedMesh, nullVertex)
DELEGATE_CONST  (ConstLinkedEdge     , WingedMesh, nullEdge)
DELEGATE_CONST  (ConstLinkedFace     , WingedMesh, nullFace)
DELEGATE        (LinkedVertex        , WingedMesh, rNullVertex)
DELEGATE        (LinkedEdge          , WingedMesh, rNullEdge)
DELEGATE        (LinkedFace          , WingedMesh, rNullFace)
DELEGATE_CONST  (ConstLinkedVertex   , WingedMesh, rNullVertex)
DELEGATE_CONST  (ConstLinkedEdge     , WingedMesh, rNullEdge)
DELEGATE_CONST  (ConstLinkedFace     , WingedMesh, rNullFace)

DELEGATE1       (void           , WingedMesh, addIndex, unsigned int)
DELEGATE2       (LinkedVertex   , WingedMesh, addVertex, const glm::vec3&, LinkedEdge)
DELEGATE1       (LinkedEdge     , WingedMesh, addEdge, const WingedEdge&)
DELEGATE1       (LinkedFace     , WingedMesh, addFace, const WingedFace&)
 
DELEGATE        (VertexIterator , WingedMesh, vertexIterator)
DELEGATE        (EdgeIterator   , WingedMesh, edgeIterator)
DELEGATE        (FaceIterator   , WingedMesh, faceIterator)

DELEGATE_CONST  (VertexConstIterator , WingedMesh, vertexIterator)
DELEGATE_CONST  (EdgeConstIterator   , WingedMesh, edgeIterator)
DELEGATE_CONST  (FaceConstIterator   , WingedMesh, faceIterator)

DELEGATE        (VertexIterator      , WingedMesh, vertexReverseIterator)
DELEGATE        (EdgeIterator        , WingedMesh, edgeReverseIterator)
DELEGATE        (FaceIterator        , WingedMesh, faceReverseIterator)

DELEGATE_CONST  (VertexConstIterator , WingedMesh, vertexReverseIterator)
DELEGATE_CONST  (EdgeConstIterator   , WingedMesh, edgeReverseIterator)
DELEGATE_CONST  (FaceConstIterator   , WingedMesh, faceReverseIterator)

DELEGATE_CONST  (unsigned int   , WingedMesh, numVertices)
DELEGATE_CONST  (unsigned int   , WingedMesh, numWingedVertices)
DELEGATE_CONST  (unsigned int   , WingedMesh, numEdges)
DELEGATE_CONST  (unsigned int   , WingedMesh, numFaces)

DELEGATE1_CONST (glm::vec3      , WingedMesh, vertex, unsigned int)

DELEGATE        (void           , WingedMesh, rebuildIndices)
DELEGATE        (void           , WingedMesh, rebuildNormals)
DELEGATE        (void           , WingedMesh, bufferData)
DELEGATE        (void           , WingedMesh, renderBegin)
DELEGATE        (void           , WingedMesh, render)
DELEGATE        (void           , WingedMesh, renderEnd)
DELEGATE        (void           , WingedMesh, reset)
DELEGATE        (void           , WingedMesh, toggleRenderMode)

DELEGATE2       (bool           , WingedMesh, intersectRay, const Ray&, FaceIntersection&)

#include <glm/glm.hpp>
#include "winged-vertex.hpp"
#include "winged-edge.hpp"
#include "winged-face.hpp"
#include "winged-mesh.hpp"
#include "winged-util.hpp"
#include "mesh.hpp"
#include "intersection.hpp"
#include "triangle.hpp"
#include "ray.hpp"
#include "macro.hpp"
#include "adjacent-iterator.hpp"
#include "octree.hpp"
#include "octree-util.hpp"

struct WingedMesh::Impl {
  WingedMesh& wingedMesh;
  Mesh        mesh;
  Vertices    vertices;
  Edges       edges;
  Octree      octree;

  Impl (WingedMesh& p) : wingedMesh (p) {}

  void addIndex (unsigned int index) { this->mesh.addIndex (index); }

  LinkedVertex addVertex (const glm::vec3& v, unsigned int l) {
    unsigned int index = this->mesh.addVertex (v);
    this->vertices.push_back (WingedVertex (index, l));
    return --this->vertices.end ();
  }

  LinkedEdge addEdge (const WingedEdge& e) {
    this->edges.push_back (e);
    return --this->edges.end ();
  }

  LinkedFace addFace (const WingedFace& f, const Triangle& geometry) {
    return this->octree.insertFace (f, geometry);
  }

  LinkedFace deleteEdge (LinkedEdge edge) {
    LinkedFace faceToDelete  = edge->rightFace ();
    LinkedFace remainingFace = edge->leftFace ();

    assert (faceToDelete->octreeNode ());

    for (LinkedEdge adjacent : faceToDelete->adjacentEdgeIterator ().collect ()) {
      adjacent->setFace (*faceToDelete, remainingFace);
    }

    edge->leftPredecessor  ()->setSuccessor   (*remainingFace, edge->rightSuccessor   ());
    edge->leftSuccessor    ()->setPredecessor (*remainingFace, edge->rightPredecessor ());

    edge->rightPredecessor ()->setSuccessor   (*remainingFace, edge->leftSuccessor   ());
    edge->rightSuccessor   ()->setPredecessor (*remainingFace, edge->leftPredecessor ());

    edge->vertex1 ()->setEdge (edge->leftPredecessor ());
    edge->vertex2 ()->setEdge (edge->leftSuccessor   ());

    if (edge->previousSibling ().isDefined ())
      edge->previousSibling ().data ()->setNextSibling (edge->nextSibling ());
    if (edge->nextSibling ().isDefined ())
      edge->nextSibling ().data ()->setPreviousSibling (edge->previousSibling ());

    remainingFace->setEdge (edge->leftSuccessor ());

    this->edges.erase (edge);
    faceToDelete->octreeNode ()->deleteFace (faceToDelete);
    return remainingFace;
  }

  LinkedFace realignInOctree (LinkedFace f) {
    OctreeNode* node = f->octreeNode ();
    if (node) {
      LinkedFace newFace = this->octree.insertFace (*f,f->triangle (this->wingedMesh));

      for (LinkedEdge e : f->adjacentEdgeIterator ().collect ())
        e->setFace (*f, newFace);

      node->deleteFace (f);
      return newFace;
    }
    return f;
  }

  unsigned int numVertices       () const { return this->mesh.numVertices (); }
  unsigned int numWingedVertices () const { return this->vertices.size    (); }
  unsigned int numEdges          () const { return this->edges.size       (); }
  unsigned int numFaces          () const { 
    return OctreeUtil :: numFaces (this->octree); 
  }

  glm::vec3 vertex (unsigned int i) const { return this->mesh.vertex (i); }

  void rebuildIndices () {
    this->mesh.clearIndices ();

    for (OctreeFaceIterator it = this->octree.faceIterator (); it.isValid (); it.next ()) {
      it.element ()->addIndices (this->wingedMesh);
    }
  }

  void rebuildNormals () {
    this->mesh.clearNormals ();
    for (WingedVertex& v : this->vertices) {
      this->mesh.addNormal (v.normal (this->wingedMesh));
    }
  }

  void bufferData  () { this->mesh.bufferData   (); }
  void render      () { 
    this->mesh.render   (); 
#ifdef DILAY_RENDER_OCTREE
    this->octree.render ();
#endif
  }

  void reset () {
    this->mesh    .reset ();
    this->vertices.clear ();
    this->edges   .clear ();
    this->octree  .reset ();
  }

  void reset (const glm::vec3& center, float width) {
    this->reset ();
    this->octree.reset (center,width);
  }

  void toggleRenderMode () { this->mesh.toggleRenderMode (); }

  void intersectRay (const Ray& ray, FaceIntersection& intersection) {
    this->octree.intersectRay (this->wingedMesh,ray,intersection);
  }

  OctreeFaceIterator octreeFaceIterator () {
    return this->octree.faceIterator ();
  }
  ConstOctreeFaceIterator octreeFaceIterator () const {
    return this->octree.faceIterator ();
  }
  OctreeNodeIterator octreeNodeIterator () {
    return this->octree.nodeIterator ();
  }
  ConstOctreeNodeIterator octreeNodeIterator () const {
    return this->octree.nodeIterator ();
  }
};

WingedMesh :: WingedMesh () : impl (new WingedMesh::Impl (*this)) {}

WingedMesh :: WingedMesh (const WingedMesh& source) 
  : impl (new WingedMesh::Impl (*this)) {
    WingedUtil :: fromMesh (*this, source.impl->mesh);
}

const WingedMesh& WingedMesh :: operator= (const WingedMesh& source) {
  if (this == &source) return *this;
  WingedUtil :: fromMesh (*this, source.impl->mesh);
  return *this;
}
DELEGATE_DESTRUCTOR (WingedMesh)

DELEGATE1       (void           , WingedMesh, addIndex, unsigned int)
DELEGATE2       (LinkedVertex   , WingedMesh, addVertex, const glm::vec3&, unsigned int)
DELEGATE1       (LinkedEdge     , WingedMesh, addEdge, const WingedEdge&)
DELEGATE2       (LinkedFace     , WingedMesh, addFace, const WingedFace&, const Triangle&)

GETTER          (const Vertices&, WingedMesh, vertices)
GETTER          (const Edges&   , WingedMesh, edges)
GETTER          (const Octree&  , WingedMesh, octree)

DELEGATE        (OctreeFaceIterator     , WingedMesh, octreeFaceIterator)
DELEGATE_CONST  (ConstOctreeFaceIterator, WingedMesh, octreeFaceIterator)
DELEGATE        (OctreeNodeIterator     , WingedMesh, octreeNodeIterator)
DELEGATE_CONST  (ConstOctreeNodeIterator, WingedMesh, octreeNodeIterator)

DELEGATE1       (LinkedFace     , WingedMesh, deleteEdge, LinkedEdge)
DELEGATE1       (LinkedFace     , WingedMesh, realignInOctree, LinkedFace)
 
DELEGATE_CONST  (unsigned int   , WingedMesh, numVertices)
DELEGATE_CONST  (unsigned int   , WingedMesh, numWingedVertices)
DELEGATE_CONST  (unsigned int   , WingedMesh, numEdges)
DELEGATE_CONST  (unsigned int   , WingedMesh, numFaces)

DELEGATE1_CONST (glm::vec3      , WingedMesh, vertex, unsigned int)

DELEGATE        (void           , WingedMesh, rebuildIndices)
DELEGATE        (void           , WingedMesh, rebuildNormals)
DELEGATE        (void           , WingedMesh, bufferData)
DELEGATE        (void           , WingedMesh, render)
DELEGATE        (void           , WingedMesh, reset)
DELEGATE2       (void           , WingedMesh, reset, const glm::vec3&, float)
DELEGATE        (void           , WingedMesh, toggleRenderMode)

DELEGATE2       (void           , WingedMesh, intersectRay, const Ray&, FaceIntersection&)

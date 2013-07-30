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
#include "util.hpp"
#include "id.hpp"

struct WingedMesh::Impl {
  WingedMesh&              wingedMesh;
  Mesh                     mesh;
  std::list <WingedVertex> vertices;
  std::list <WingedEdge>   edges;
  Octree                   octree;

  Impl (WingedMesh& p) : wingedMesh (p) {}

  void addIndex (unsigned int index) { this->mesh.addIndex (index); }

  WingedVertex& addVertex (const glm::vec3& v, unsigned int l) {
    unsigned int index = this->mesh.addVertex (v);
    this->vertices.push_back (WingedVertex (index, nullptr, l));
    return this->vertices.back ();
  }

  WingedEdge& addEdge (const WingedEdge& e) {
    this->edges.push_back (e);
    return this->edges.back ();
  }

  WingedFace& addFace (const WingedFace& f, const Triangle& geometry) {
    return this->octree.insertNewFace (f, geometry);
  }

  WingedFace& deleteEdge (WingedEdge& edge) {
    WingedFace* faceToDelete  = edge.rightFace ();
    WingedFace* remainingFace = edge.leftFace ();

    assert (faceToDelete->octreeNode ());

    for (auto it = faceToDelete->adjacentEdgeIterator (); it.isValid (); ) {
      WingedEdge& adjacent = it.element ();
      it.next ();
      adjacent.face (*faceToDelete, remainingFace);
    }

    edge.leftPredecessorRef  ().successor   (*remainingFace, edge.rightSuccessor   ());
    edge.leftSuccessorRef    ().predecessor (*remainingFace, edge.rightPredecessor ());

    edge.rightPredecessorRef ().successor   (*remainingFace, edge.leftSuccessor   ());
    edge.rightSuccessorRef   ().predecessor (*remainingFace, edge.leftPredecessor ());

    edge.vertex1Ref ().edge (edge.leftPredecessor ());
    edge.vertex2Ref ().edge (edge.leftSuccessor   ());

    if (edge.previousSibling ())
      edge.previousSiblingRef ().nextSibling (edge.nextSibling ());
    if (edge.nextSibling ())
      edge.nextSiblingRef ().previousSibling (edge.previousSibling ());

    remainingFace->edge (edge.leftSuccessor ());

    Util :: eraseByAddress <WingedEdge> (this->edges, &edge);
    this->octree.deleteFace (*faceToDelete); 
    return *remainingFace;
  }

  WingedFace& realignInOctree (WingedFace& f) {
    WingedFace& newFace = this->octree.reInsertFace (f,f.triangle (this->wingedMesh));

    for (auto it = f.adjacentEdgeIterator (); it.isValid (); ) {
      WingedEdge& adjacent = it.element ();
      it.next ();
      adjacent.face (f,&newFace);
    }
    //this->octree.deleteFace (f);
    return newFace;
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
      it.element ().addIndices (this->wingedMesh);
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
DELEGATE2       (WingedVertex&  , WingedMesh, addVertex, const glm::vec3&, unsigned int)
DELEGATE1       (WingedEdge&    , WingedMesh, addEdge, const WingedEdge&)
DELEGATE2       (WingedFace&    , WingedMesh, addFace, const WingedFace&, const Triangle&)

GETTER          (const std::list <WingedVertex>&, WingedMesh, vertices)
GETTER          (const std::list <WingedEdge>&  , WingedMesh, edges)
GETTER          (const Octree&                  , WingedMesh, octree)

DELEGATE        (OctreeFaceIterator     , WingedMesh, octreeFaceIterator)
DELEGATE_CONST  (ConstOctreeFaceIterator, WingedMesh, octreeFaceIterator)
DELEGATE        (OctreeNodeIterator     , WingedMesh, octreeNodeIterator)
DELEGATE_CONST  (ConstOctreeNodeIterator, WingedMesh, octreeNodeIterator)

DELEGATE1       (WingedFace&    , WingedMesh, deleteEdge, WingedEdge&)
DELEGATE1       (WingedFace&    , WingedMesh, realignInOctree, WingedFace&)
 
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

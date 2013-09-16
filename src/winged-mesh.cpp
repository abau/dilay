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
#include "id.hpp"

struct WingedMesh::Impl {
  const IdObject           id;
  WingedMesh&              wingedMesh;
  Mesh                     mesh;
  std::list <WingedVertex> vertices;
  std::list <WingedEdge>   edges;
  Octree                   octree;
  std::list <unsigned int> freeFirstIndexNumbers;

  Impl (WingedMesh& p) : wingedMesh (p) {}

  glm::vec3 vertex (unsigned int i) const { return this->mesh.vertex (i); }

  WingedFace& face (const Id& id) const { return this->octree.faceRef (id); }

  unsigned int addIndex (unsigned int index) { 
    return this->mesh.addIndex (index); 
  }

  WingedVertex& addVertex (const glm::vec3& v, unsigned int l) {
    unsigned int index = this->mesh.addVertex (v);
    this->vertices.push_back (WingedVertex (index, nullptr, l));
    return this->vertices.back ();
  }

  WingedEdge& addEdge (const WingedEdge& e) {
    this->edges.push_back (e);
    this->edges.back ().iterator (--this->edges.end ());
    return this->edges.back ();
  }

  WingedFace& addFace (const WingedFace& f, const Triangle& geometry) {
    return this->octree.insertNewFace (f, geometry);
  }

  void setIndex (unsigned int indexNumber, unsigned int index) { 
    return this->mesh.setIndex (indexNumber, index); 
  }

  void setVertex (unsigned int index, const glm::vec3& v) {
    return this->mesh.setVertex (index,v);
  }

  void setNormal (unsigned int index, const glm::vec3& n) {
    return this->mesh.setNormal (index,n);
  }

  void releaseFirstIndexNumber (WingedFace& face) {
    if (face.firstIndexNumber ().isDefined ()) {
      this->freeFirstIndexNumbers.push_back (face.firstIndexNumber ().data ());
      face.resetFirstIndexNumber ();
    }
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

    this->releaseFirstIndexNumber (*faceToDelete);
    this->releaseFirstIndexNumber (*remainingFace);
    this->edges.erase             (edge.iterator ());
    this->octree.deleteFace       (*faceToDelete); 
    return *remainingFace;
  }

  WingedFace& realignInOctree (WingedFace& f) {
    WingedFace& newFace = this->octree.reInsertFace (f,f.triangle (this->wingedMesh));

    for (auto it = f.adjacentEdgeIterator (); it.isValid (); ) {
      WingedEdge& adjacent = it.element ();
      it.next ();
      adjacent.face (f,&newFace);
    }
    return newFace;
  }

  unsigned int numVertices       () const { return this->mesh.numVertices (); }
  unsigned int numWingedVertices () const { return this->vertices.size    (); }
  unsigned int numEdges          () const { return this->edges.size       (); }
  unsigned int numFaces          () const { 
    return OctreeUtil :: numFaces (this->octree); 
  }
  unsigned int numIndices        () const { return this->mesh.numIndices  (); }

  void write () {
    // Indices
    for (OctreeFaceIterator it = this->octree.faceIterator (); it.isValid (); it.next ()) {
      it.element ().writeIndices          (this->wingedMesh);
    }
    // Normals
    for (WingedVertex& v : this->vertices) {
      v.writeNormal (this->wingedMesh);
    }
  }

  void bufferData  () { 
    assert (this->freeFirstIndexNumbers.size () == 0);
    this->mesh.bufferData   (); 
  }

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

  void intersectSphere (const Sphere& sphere, std::list <Id>& ids) {
    this->octree.intersectSphere (this->wingedMesh,sphere,ids);
  }

  void intersectSphere (const Sphere& sphere, std::unordered_set <WingedVertex*>& vertices) {
    this->octree.intersectSphere (this->wingedMesh,sphere,vertices);
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

  bool hasFreeFirstIndexNumber () const { 
    return this->freeFirstIndexNumbers.size () > 0;
  }
  
  unsigned int nextFreeFirstIndexNumber () {
    unsigned int indexNumber = this->freeFirstIndexNumbers.front ();
    this->freeFirstIndexNumbers.pop_front ();
    return indexNumber;
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
ID                  (WingedMesh)

DELEGATE1_CONST (glm::vec3      , WingedMesh, vertex, unsigned int)
DELEGATE1_CONST (WingedFace&    , WingedMesh, face, const Id&)

DELEGATE1       (unsigned int   , WingedMesh, addIndex, unsigned int)
DELEGATE2       (WingedVertex&  , WingedMesh, addVertex, const glm::vec3&, unsigned int)
DELEGATE1       (WingedEdge&    , WingedMesh, addEdge, const WingedEdge&)
DELEGATE2       (WingedFace&    , WingedMesh, addFace, const WingedFace&, const Triangle&)
DELEGATE2       (void           , WingedMesh, setIndex, unsigned int, unsigned int)
DELEGATE2       (void           , WingedMesh, setVertex, unsigned int, const glm::vec3&)
DELEGATE2       (void           , WingedMesh, setNormal, unsigned int, const glm::vec3&)

GETTER          (const Vertices&, WingedMesh, vertices)
GETTER          (const Edges&   , WingedMesh, edges)
GETTER          (const Octree&  , WingedMesh, octree)

DELEGATE        (OctreeFaceIterator     , WingedMesh, octreeFaceIterator)
DELEGATE_CONST  (ConstOctreeFaceIterator, WingedMesh, octreeFaceIterator)
DELEGATE        (OctreeNodeIterator     , WingedMesh, octreeNodeIterator)
DELEGATE_CONST  (ConstOctreeNodeIterator, WingedMesh, octreeNodeIterator)


DELEGATE1       (void           , WingedMesh, releaseFirstIndexNumber, WingedFace&)
DELEGATE1       (WingedFace&    , WingedMesh, deleteEdge, WingedEdge&)
DELEGATE1       (WingedFace&    , WingedMesh, realignInOctree, WingedFace&)
 
DELEGATE_CONST  (unsigned int   , WingedMesh, numVertices)
DELEGATE_CONST  (unsigned int   , WingedMesh, numWingedVertices)
DELEGATE_CONST  (unsigned int   , WingedMesh, numEdges)
DELEGATE_CONST  (unsigned int   , WingedMesh, numFaces)
DELEGATE_CONST  (unsigned int   , WingedMesh, numIndices)

DELEGATE        (void , WingedMesh, write)
DELEGATE        (void , WingedMesh, bufferData)
DELEGATE        (void , WingedMesh, render)
DELEGATE        (void , WingedMesh, reset)
DELEGATE2       (void , WingedMesh, reset, const glm::vec3&, float)
DELEGATE        (void , WingedMesh, toggleRenderMode)

DELEGATE2       (void , WingedMesh, intersectRay, const Ray&, FaceIntersection&)
DELEGATE2       (void , WingedMesh, intersectSphere, const Sphere&, std::list<Id>&)
DELEGATE2       (void , WingedMesh, intersectSphere, const Sphere&, std::unordered_set<WingedVertex*>&)

DELEGATE_CONST  (bool         , WingedMesh, hasFreeFirstIndexNumber)
DELEGATE        (unsigned int , WingedMesh, nextFreeFirstIndexNumber)

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

struct WingedMeshImpl {
  WingedMesh& wingedMesh;
  Mesh        mesh;
  Vertices    vertices;
  Edges       edges;
  Faces       faces;
  Octree      octree;

  WingedMeshImpl (WingedMesh& p) : wingedMesh (p) {}

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
    this->faces.push_back (f);
    this->octree.insertFace (f, geometry);
    return --this->faces.end ();
  }

  LinkedFace deleteEdge (LinkedEdge edge) {
    LinkedFace faceToDelete  = edge->rightFace ();
    LinkedFace remainingFace = edge->leftFace ();

    for (LinkedEdge adjacent : faceToDelete->adjacentEdgeIterator ().collect ()) {
      adjacent->setFace (*faceToDelete, remainingFace);
    }

    edge->leftPredecessor ()->setSuccessor   (*remainingFace, edge->rightSuccessor   ());
    edge->leftSuccessor   ()->setPredecessor (*remainingFace, edge->rightPredecessor ());

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
    this->faces.erase (faceToDelete);
    return remainingFace;
  }

  unsigned int numVertices       () const { return this->mesh.numVertices (); }
  unsigned int numWingedVertices () const { return this->vertices.size    (); }
  unsigned int numEdges          () const { return this->edges.size       (); }
  unsigned int numFaces          () const { return this->faces.size       (); }

  glm::vec3 vertex (unsigned int i) const { return this->mesh.vertex (i); }

  void rebuildIndices () {
    this->mesh.clearIndices ();
    for (WingedFace& f : this->faces) {
      f.addIndices (this->wingedMesh);
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
    this->faces   .clear ();
  }

  void toggleRenderMode () { this->mesh.toggleRenderMode (); }

  bool intersectRay (const Ray& ray, FaceIntersection& intersection) {
    bool      isIntersection = false;
    float     minDistance    = 0.0f;
    Triangle  triangle;
    glm::vec3 p;

    for (LinkedFace it = this->faces.begin (); it != this->faces.end (); ++it) {
      it->triangle (this->wingedMesh,triangle);
      if (triangle.intersectRay (ray,p) == false) {
        continue;
      }
      else {
        float d = glm::distance (ray.origin (), p);

        if (isIntersection == false || d < minDistance) {
          isIntersection        = true;
          minDistance           = d;
          intersection.position (p);
          intersection.face     (it);
        }
      }
    }
    return isIntersection;
  }
};

WingedMesh :: WingedMesh () : impl (new WingedMeshImpl (*this)) {}

DELEGATE_COPY_CONSTRUCTOR (WingedMesh)
DELEGATE_ASSIGNMENT_OP    (WingedMesh)
DELEGATE_DESTRUCTOR       (WingedMesh)

DELEGATE1       (void           , WingedMesh, addIndex, unsigned int)
DELEGATE2       (LinkedVertex   , WingedMesh, addVertex, const glm::vec3&, unsigned int)
DELEGATE1       (LinkedEdge     , WingedMesh, addEdge, const WingedEdge&)
DELEGATE2       (LinkedFace     , WingedMesh, addFace, const WingedFace&, const Triangle&)

GETTER          (const Vertices&, WingedMesh, vertices)
GETTER          (const Edges&   , WingedMesh, edges)
GETTER          (const Faces&   , WingedMesh, faces)
GETTER          (const Octree&  , WingedMesh, octree)

DELEGATE1       (LinkedFace     , WingedMesh, deleteEdge, LinkedEdge)
 
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
DELEGATE        (void           , WingedMesh, toggleRenderMode)

DELEGATE2       (bool           , WingedMesh, intersectRay, const Ray&, FaceIntersection&)

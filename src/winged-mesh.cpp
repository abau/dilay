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

struct WingedMeshImpl {
  WingedMesh& _wingedMesh;
  Mesh        _mesh;
  Vertices    _vertices;
  Edges       _edges;
  Faces       _faces;

  WingedMeshImpl (WingedMesh& p) : _wingedMesh (p) {}

  void addIndex (unsigned int index) { this->_mesh.addIndex (index); }

  LinkedVertex addVertex (const glm::vec3& v, unsigned int l) {
    unsigned int index = this->_mesh.addVertex (v);
    this->_vertices.push_back (WingedVertex (index, l));
    return --this->_vertices.end ();
  }

  LinkedEdge addEdge (const WingedEdge& e) {
    this->_edges.push_back (e);
    return --this->_edges.end ();
  }

  LinkedFace addFace (const WingedFace& f) {
    this->_faces.push_back (f);
    return --this->_faces.end ();
  }

  const Vertices& vertices () const { return this->_vertices; }
  const Edges&    edges    () const { return this->_edges; }
  const Faces&    faces    () const { return this->_faces; }

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

    this->_edges.erase (edge);
    this->_faces.erase (faceToDelete);
    return remainingFace;
  }

  unsigned int numVertices       () const { return this->_mesh.numVertices (); }
  unsigned int numWingedVertices () const { return this->_vertices.size    (); }
  unsigned int numEdges          () const { return this->_edges.size       (); }
  unsigned int numFaces          () const { return this->_faces.size       (); }

  glm::vec3 vertex (unsigned int i) const { return this->_mesh.vertex (i); }

  void rebuildIndices () {
    this->_mesh.clearIndices ();
    for (WingedFace& f : this->_faces) {
      f.addIndices (this->_wingedMesh);
    }
  }

  void rebuildNormals () {
    this->_mesh.clearNormals ();
    for (WingedVertex& v : this->_vertices) {
      this->_mesh.addNormal (v.normal (this->_wingedMesh));
    }
  }

  void bufferData  () { this->_mesh.bufferData   (); }
  void renderBegin () { this->_mesh.renderBegin  (); }
  void render      () { this->_mesh.render       (); }
  void renderEnd   () { this->_mesh.renderEnd    (); }

  void reset () {
    this->_mesh    .reset ();
    this->_vertices.clear ();
    this->_edges   .clear ();
    this->_faces   .clear ();
  }

  void toggleRenderMode () { this->_mesh.toggleRenderMode (); }

  bool intersectRay (const Ray& ray, FaceIntersection& intersection) {
    bool      isIntersection = false;
    float     minDistance    = 0.0f;
    Triangle  triangle;
    glm::vec3 p;

    for (LinkedFace it = this->_faces.begin (); it != this->_faces.end (); ++it) {
      it->triangle (this->_wingedMesh,triangle);
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
DELEGATE1       (LinkedFace     , WingedMesh, addFace, const WingedFace&)

DELEGATE_CONST  (const Vertices&, WingedMesh, vertices)
DELEGATE_CONST  (const Edges&   , WingedMesh, edges)
DELEGATE_CONST  (const Faces&   , WingedMesh, faces)

DELEGATE1       (LinkedFace     , WingedMesh, deleteEdge, LinkedEdge)
 
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

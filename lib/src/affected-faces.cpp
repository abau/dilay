#include "adjacent-iterator.hpp"
#include "affected-faces.hpp"
#include "winged/edge.hpp"
#include "winged/face.hpp"
#include "winged/vertex.hpp"

struct AffectedFaces::Impl {
  FacePtrSet faces;
  FacePtrSet uncommitedFaces;

  void insert (WingedFace& face) {
    this->uncommitedFaces.insert (&face);
  }

  void insert (const AffectedFaces& faces) {
    for (WingedFace* f : faces.faces ()) {
      this->insert (*f);
    }
    for (WingedFace* f : faces.uncommitedFaces ()) {
      this->insert (*f);
    }
  }

  void remove (WingedFace& face) {
    this->uncommitedFaces.erase (&face);
    this->faces          .erase (&face);
  }

  void reset () {
    this->faces          .clear ();
    this->uncommitedFaces.clear ();
  }

  void commit () { 
    for (WingedFace* f : this->uncommitedFaces) {
      this->faces.insert (f);
    }
    this->uncommitedFaces.clear ();
  }

  bool isEmpty () const {
    return this->faces.empty () && this->uncommitedFaces.empty ();
  }

  bool contains (WingedFace& face) const { 
    return this->contains (&face);
  }

  bool contains (WingedFace* face) const { 
    return this->faces.count (face) > 0 || this->uncommitedFaces.count (face) > 0;
  }

  VertexPtrSet toVertexSet () const {
    VertexPtrSet vertices;
    for (WingedFace* f : this->faces) {
      for (WingedEdge& e : f->adjacentEdges ()) {
        vertices.insert (e.vertex1 ());
        vertices.insert (e.vertex2 ());
      }
    }
    return vertices;
  }

  EdgePtrVec toEdgeVec () const {
    EdgePtrVec edges;
    for (WingedFace* f : this->faces) {
      for (WingedEdge& e : f->adjacentEdges ()) {
        if (e.isLeftFace (*f) || (this->faces.count (e.otherFace (*f)) == 0)) {
          edges.push_back (&e);
        }
      }
    }
    return edges;
  }
};

DELEGATE_BIG6   (AffectedFaces)
DELEGATE1       (void, AffectedFaces, insert, WingedFace&)
DELEGATE1       (void, AffectedFaces, insert, const AffectedFaces&)
DELEGATE1       (void, AffectedFaces, remove, WingedFace&)
DELEGATE        (void, AffectedFaces, reset)
DELEGATE        (void, AffectedFaces, commit)
DELEGATE_CONST  (bool, AffectedFaces, isEmpty)
DELEGATE1_CONST (bool, AffectedFaces, contains, WingedFace&)
DELEGATE1_CONST (bool, AffectedFaces, contains, WingedFace*)
GETTER_CONST    (const FacePtrSet&, AffectedFaces, faces)
GETTER_CONST    (const FacePtrSet&, AffectedFaces, uncommitedFaces)
DELEGATE_CONST  (VertexPtrSet, AffectedFaces, toVertexSet)
DELEGATE_CONST  (EdgePtrVec, AffectedFaces, toEdgeVec)

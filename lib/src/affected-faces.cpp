#include "adjacent-iterator.hpp"
#include "affected-faces.hpp"
#include "winged/edge.hpp"
#include "winged/face.hpp"
#include "winged/vertex.hpp"

struct AffectedFaces::Impl {
  FaceSet   faces;
  EdgeSet   edges;
  FaceSet   uncommitedFaces;

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

    for (WingedEdge& e : face.adjacentEdges ()) {
      if (this->contains (e.otherFaceRef (face)) == false) {
        this->remove (e);
      }
    }
  }

  void remove (WingedEdge& edge) {
    this->edges.erase (&edge);
  }

  void reset () {
    this->faces          .clear ();
    this->edges          .clear ();
    this->uncommitedFaces.clear ();
  }

  void commit () { 
    for (WingedFace* f : this->uncommitedFaces) {
      this->faces.insert (f);

      for (WingedEdge& e : f->adjacentEdges ()) {
        this->edges.insert (&e);
      }
    }
    this->uncommitedFaces.clear ();
  }

  bool contains (WingedFace& face) const { 
    return this->contains (&face);
  }

  bool contains (WingedFace* face) const { 
    return this->faces.count (face) > 0 || this->uncommitedFaces.count (face) > 0;
  }

  bool contains (WingedEdge& edge) const { 
    return this->contains (&edge);
  }

  bool contains (WingedEdge* edge) const { 
    return this->edges.count (edge) > 0;
  }

  VertexSet toVertexSet () const {
    VertexSet vertices;
    for (WingedEdge* e : this->edges) {
      vertices.insert (e->vertex1 ());
      vertices.insert (e->vertex2 ());
    }
    return vertices;
  }
};

DELEGATE_BIG6   (AffectedFaces)
DELEGATE1       (void, AffectedFaces, insert, WingedFace&)
DELEGATE1       (void, AffectedFaces, insert, const AffectedFaces&)
DELEGATE1       (void, AffectedFaces, remove, WingedFace&)
DELEGATE1       (void, AffectedFaces, remove, WingedEdge&)
DELEGATE        (void, AffectedFaces, reset)
DELEGATE        (void, AffectedFaces, commit)
DELEGATE1_CONST (bool, AffectedFaces, contains, WingedFace&)
DELEGATE1_CONST (bool, AffectedFaces, contains, WingedFace*)
DELEGATE1_CONST (bool, AffectedFaces, contains, WingedEdge&)
DELEGATE1_CONST (bool, AffectedFaces, contains, WingedEdge*)
DELEGATE_CONST  (VertexSet, AffectedFaces, toVertexSet)
GETTER_CONST    (FaceSet&, AffectedFaces, faces)
GETTER_CONST    (FaceSet&, AffectedFaces, uncommitedFaces)
GETTER_CONST    (EdgeSet&, AffectedFaces, edges)

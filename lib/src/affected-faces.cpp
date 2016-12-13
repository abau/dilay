/* This file is part of Dilay
 * Copyright © 2015,2016 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include "adjacent-iterator.hpp"
#include "affected-faces.hpp"
#include "winged/edge.hpp"
#include "winged/face.hpp"

struct AffectedFaces::Impl {
  FacePtrSet faces;
  FacePtrSet uncommittedFaces;

  void insert (WingedFace& face) {
    this->uncommittedFaces.insert (&face);
  }

  void insert (const AffectedFaces& faces) {
    for (WingedFace* f : faces.faces ()) {
      this->insert (*f);
    }
    for (WingedFace* f : faces.uncommittedFaces ()) {
      this->insert (*f);
    }
  }

  void remove (WingedFace& face) {
    this->uncommittedFaces.erase (&face);
    this->faces           .erase (&face);
  }

  void reset () {
    this->faces           .clear ();
    this->uncommittedFaces.clear ();
  }

  void resetCommitted () {
    this->faces.clear ();
  }

  void commit () { 
    this->faces.insert (this->uncommittedFaces.begin (), this->uncommittedFaces.end ());
    this->uncommittedFaces.clear ();
  }

  bool isEmpty () const {
    return this->faces.empty () && this->uncommittedFaces.empty ();
  }

  bool contains (WingedFace& face) const { 
    return this->contains (&face);
  }

  bool contains (WingedFace* face) const { 
    return this->faces.count (face) > 0 || this->uncommittedFaces.count (face) > 0;
  }

  void filter (const std::function <bool (const WingedFace&)>& f) {
    for (auto it = this->faces.begin (); it != this->faces.end (); ) {
      if (f (**it)) {
        it = this->faces.erase (it);
      }
      else {
        ++it;
      }
    }
    for (auto it = this->uncommittedFaces.begin (); it != this->uncommittedFaces.end (); ) {
      if (f (**it)) {
        it = this->uncommittedFaces.erase (it);
      }
      else {
        ++it;
      }
    }
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
DELEGATE1       (void,              AffectedFaces, insert, WingedFace&)
DELEGATE1       (void,              AffectedFaces, insert, const AffectedFaces&)
DELEGATE1       (void,              AffectedFaces, remove, WingedFace&)
DELEGATE        (void,              AffectedFaces, reset)
DELEGATE        (void,              AffectedFaces, resetCommitted)
DELEGATE        (void,              AffectedFaces, commit)
DELEGATE_CONST  (bool,              AffectedFaces, isEmpty)
DELEGATE1_CONST (bool,              AffectedFaces, contains, WingedFace&)
DELEGATE1_CONST (bool,              AffectedFaces, contains, WingedFace*)
DELEGATE1       (void,              AffectedFaces, filter, const std::function <bool (const WingedFace&)>&)
GETTER_CONST    (const FacePtrSet&, AffectedFaces, faces)
GETTER_CONST    (const FacePtrSet&, AffectedFaces, uncommittedFaces)
DELEGATE_CONST  (VertexPtrSet,      AffectedFaces, toVertexSet)
DELEGATE_CONST  (EdgePtrVec,        AffectedFaces, toEdgeVec)

/* This file is part of Dilay
 * Copyright © 2015 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include "adjacent-iterator.hpp"
#include "affected-faces.hpp"
#include "primitive/triangle.hpp"
#include "winged/edge.hpp"
#include "winged/face.hpp"

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

  void discardBackfaces (const WingedMesh& mesh, const glm::vec3& normal) {
    auto discard = [&mesh, &normal] (WingedFace* f) {
      return glm::dot (normal, f->triangle (mesh).cross ()) <= 0.0f;
    };

    for (auto it = faces.begin (); it != faces.end (); ) {
      if (discard (*it)) {
        it = this->faces.erase (it);
      }
      else {
        ++it;
      }
    }
    for (auto it = uncommitedFaces.begin (); it != uncommitedFaces.end (); ) {
      if (discard (*it)) {
        it = this->uncommitedFaces.erase (it);
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
DELEGATE        (void,              AffectedFaces, commit)
DELEGATE_CONST  (bool,              AffectedFaces, isEmpty)
DELEGATE1_CONST (bool,              AffectedFaces, contains, WingedFace&)
DELEGATE1_CONST (bool,              AffectedFaces, contains, WingedFace*)
DELEGATE2       (void,              AffectedFaces, discardBackfaces, const WingedMesh&, const glm::vec3&)
GETTER_CONST    (const FacePtrSet&, AffectedFaces, faces)
GETTER_CONST    (const FacePtrSet&, AffectedFaces, uncommitedFaces)
DELEGATE_CONST  (VertexPtrSet,      AffectedFaces, toVertexSet)
DELEGATE_CONST  (EdgePtrVec,        AffectedFaces, toEdgeVec)

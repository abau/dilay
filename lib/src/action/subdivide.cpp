#include <glm/glm.hpp>
#include "action/subdivide.hpp"
#include "action/unit/on.hpp"
#include "adjacent-iterator.hpp"
#include "partial-action/insert-edge-vertex.hpp"
#include "partial-action/triangulate-6-gon.hpp"
#include "partial-action/triangulate-quad.hpp"
#include "winged/edge.hpp"
#include "winged/face.hpp"
#include "winged/mesh.hpp"
#include "winged/vertex.hpp"

namespace {
  typedef std::unordered_set <WingedFace*> FaceSet;

  struct SubdivideData {
    WingedMesh&                    mesh;
    const std::unordered_set <Id>& selection;
    std::vector <Id>*              affectedFaces;

    SubdivideData (WingedMesh& m, const std::unordered_set <Id>& s, std::vector <Id>* n) 
      : mesh           (m)
      , selection      (s)
      , affectedFaces  (n)
    {}
  };
};

struct ActionSubdivide::Impl {
  ActionSubdivide*          self;
  ActionUnitOn <WingedMesh> actions;

  Impl (ActionSubdivide* s) : self (s) {}

  void runUndoBeforePostProcessing (WingedMesh& mesh) { this->actions.undo (mesh); }
  void runRedoBeforePostProcessing (WingedMesh& mesh) { this->actions.redo (mesh); }

  void run ( WingedMesh& mesh, const std::unordered_set <Id>& selection
           , std::vector <Id>* affected ) 
  {
    this->subdivide (SubdivideData (mesh, selection, affected));
  }

  void subdivide (const SubdivideData& data) {
    FaceSet neighbourhood, border;

    this->addOneRing          (      neighbourhood);
    this->addOneRing          (      neighbourhood);
    this->extendNeighbourhood (data, neighbourhood);
    this->oneRingBorder       (      neighbourhood, border);
    this->subdivideFaces      (data, neighbourhood);
    this->refineBorder        (data, border);
    this->realignFaces        (data, std::move (neighbourhood));
    this->realignFaces        (data, std::move (border));
  }

  void extendNeighbourhood (const SubdivideData& data, FaceSet& neighbourhood) {
    auto hasAtLeast2Neighbours = [&neighbourhood] (const WingedFace& face) -> bool {
      unsigned int numNeighbours = 0;

      for (WingedFace& a : face.adjacentFaces ()) {
        if (neighbourhood.count (&a) > 0) {
          numNeighbours++;
        }
      }
      return numNeighbours >= 2;
    };

    auto hasPoleVertex = [] (const WingedFace& face) -> bool {
      for (WingedVertex& v : face.adjacentVertices ()) {
        if (v.valence () > 9) {
          return true;
        }
      }
      return false;
    };

    auto extendTo = [&] (const WingedFace& face) -> bool {
      return hasAtLeast2Neighbours (face) || hasPoleVertex (face);
    };

    // checks adjacent faces of a neighbour
    std::function <void (WingedFace&)> checkAdjacents = [&] (WingedFace& neighbour) -> void
    {
      for (WingedFace& face : neighbour.adjacentFaces ()) {
        if (neighbourhood.count (&face) == 0 && extendTo (face)) {
          neighbourhood.insert (&face);
          checkAdjacents (face);
        }
      }
    };

    neighbourhood.clear  ();
    for (const Id& id : data.selection) {
      WingedFace& face = data.mesh.faceRef (id);
      neighbourhood.insert (&face);
      checkAdjacents       ( face);
    }
  }

  void addOneRing (FaceSet& neighbourhood) {
    FaceSet oneRing;
    for (WingedFace* n : neighbourhood) {
      for (WingedFace& face : n->adjacentFaces ()) {
        if (neighbourhood.count (&face) == 0) {
          oneRing.insert (&face);
        }
      }
    }
    neighbourhood.insert (oneRing.begin (), oneRing.end ());
  }

  void oneRingBorder (const FaceSet& neighbourhood, FaceSet& border) {
    border.clear ();
    for (WingedFace* n : neighbourhood) {
      for (WingedFace& face : n->adjacentFaces ()) {
        if (neighbourhood.count (&face) == 0) {
          border.insert (&face);
        }
      }
    }
  }

  void subdivideFaces (const SubdivideData& data, FaceSet& faces) {
    std::unordered_set<Id> subdividedEdges;

    for (WingedFace* face : faces) {
      for (WingedEdge* edge : face->adjacentEdges ().collect ()) {
        if (subdividedEdges.count (edge->id ()) == 0) {
          WingedEdge& newEdge = this->actions.add <PAInsertEdgeVertex> ().run 
            (data.mesh, *edge, edge->middle (data.mesh));

          subdividedEdges.insert (edge  ->id ());
          subdividedEdges.insert (newEdge.id ());
        }
      }
      this->actions.add <PATriangulate6Gon> ().run (data.mesh, *face, data.affectedFaces);
    }
  }

  void refineBorder (const SubdivideData& data, FaceSet& border) {
    for (WingedFace* face : border) {
      this->actions.add <PATriangulateQuad> ().run (data.mesh, *face, data.affectedFaces);
    }
  }
 
  void realignFaces (const SubdivideData& data, FaceSet&& faces) {
    for (WingedFace* f : faces) {
      assert (f->isTriangle ());
      this->self->realignFace (data.mesh, std::move (*f));
    }
    faces.clear ();
  }

  /*
  WingedEdge& run (WingedMesh& mesh, WingedEdge& edge) { 

    WingedEdge& newE = actions.add <PAInsertEdgeVertex> ().run (mesh, edge, edge.middle (mesh));
    actions.add <PATriangulateQuad> ().run (mesh, newE.leftFaceRef  (), newE.vertex2Ref ());
    actions.add <PATriangulateQuad> ().run (mesh, newE.rightFaceRef (), newE.vertex2Ref ());
    return newE;
  }
  */
};

DELEGATE_BIG3_SELF (ActionSubdivide)
DELEGATE3          (void, ActionSubdivide, run, WingedMesh&, const std::unordered_set<Id>&, std::vector<Id>*)
DELEGATE1          (void, ActionSubdivide, runUndoBeforePostProcessing, WingedMesh&)
DELEGATE1          (void, ActionSubdivide, runRedoBeforePostProcessing, WingedMesh&)

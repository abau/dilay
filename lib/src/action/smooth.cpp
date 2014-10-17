#include <unordered_map>
#include <glm/glm.hpp>
#include "action/smooth.hpp"
#include "action/unit/on.hpp"
#include "adjacent-iterator.hpp"
#include "affected-faces.hpp"
#include "intersection.hpp"
#include "partial-action/modify-winged-vertex.hpp"
#include "primitive/triangle.hpp"
#include "winged/edge.hpp"
#include "winged/face.hpp"
#include "winged/vertex.hpp"

struct ActionSmooth::Impl {
  ActionUnitOn <WingedMesh> actions;

  void runUndo (WingedMesh& mesh) { this->actions.undo (mesh); }
  void runRedo (WingedMesh& mesh) { this->actions.redo (mesh); }

  void run (WingedMesh& mesh, const VertexSet& vertices, AffectedFaces& affectedFaces) {
    std::unordered_map <WingedVertex*,glm::vec3> deltas;
    // compute deltas
    for (WingedVertex* v : vertices) {
      const glm::vec3    p       (v->vector (mesh));
            glm::vec3    delta   (0.0f);
            unsigned int valence (0);

      for (WingedEdge& e : v->adjacentEdges ()) {
        delta += e.otherVertexRef (*v).vector (mesh) - p;
        valence++;
      }
      deltas.emplace (v, delta / float (valence));
    }
    // compute positions
    std::unordered_map <WingedVertex*,glm::vec3> positions;
    for (auto deltaIt : deltas) {
      WingedVertex& v (*deltaIt.first);
      glm::vec3     normal  (0.0f);
      unsigned int  valence (0);

      for (WingedFace& f : v.adjacentFaces ()) {
        PrimTriangle triangle (f.triangle (mesh));

        if (triangle.isDegenerated () == false) {
          normal += triangle.normal ();
          valence++;
        }
      }
      normal *= 1.0f / float (valence);

      const glm::vec3 p     (v.vector (mesh));
      const glm::vec3 delta (deltaIt.second);
      const float     dot   (glm::dot (normal, delta));
      const glm::vec3 newP  ((p + delta) - (normal * dot));

      positions.emplace (&v, newP);
    }
    // move & add affected faces
    for (auto it : positions) {
      this->actions.add <PAModifyWVertex> ().move (mesh, *it.first, it.second);

      for (WingedFace& f : it.first->adjacentFaces ()) {
        affectedFaces.insert (f);
      }
    }
  }
};

DELEGATE_BIG3 (ActionSmooth)
DELEGATE3 (void, ActionSmooth, run, WingedMesh&, const VertexSet&, AffectedFaces&)
DELEGATE1 (void, ActionSmooth, runUndo, WingedMesh&)
DELEGATE1 (void, ActionSmooth, runRedo, WingedMesh&)

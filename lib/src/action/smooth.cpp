#include <glm/glm.hpp>
#include "action/smooth.hpp"
#include "action/unit/on.hpp"
#include "adjacent-iterator.hpp"
#include "affected-faces.hpp"
#include "intersection.hpp"
#include "partial-action/modify-winged-vertex.hpp"
#include "primitive/ray.hpp"
#include "primitive/triangle.hpp"
#include "winged/edge.hpp"
#include "winged/face.hpp"
#include "winged/mesh.hpp"
#include "winged/vertex.hpp"

struct ActionSmooth::Impl {
  ActionUnitOn <WingedMesh> actions;

  void runUndo (WingedMesh& mesh) const { this->actions.undo (mesh); }
  void runRedo (WingedMesh& mesh) const { this->actions.redo (mesh); }

  void run ( WingedMesh& mesh, const VertexPtrSet& vertices, unsigned int numIterations
           , AffectedFaces& affectedFaces ) 
  {
    std::vector <glm::vec3> originalPositions;
    for (WingedVertex* v : vertices) {
      originalPositions.push_back (v->position (mesh));
    }

    for (unsigned int i = 0; i < numIterations; i++) {
      this->iteration (mesh, vertices);
    }

    auto posIt = originalPositions.begin ();
    for (WingedVertex* v : vertices) {
      assert (posIt != originalPositions.end ());

      this->actions.add <PAModifyWVertex> ().moved (mesh, *v, *posIt);

      for (WingedFace& f : v->adjacentFaces ()) {
        affectedFaces.insert (f);
      }
      ++posIt;
    }
  }

  void iteration (WingedMesh& mesh, const VertexPtrSet& vertices) {
    typedef std::vector <PrimTriangle> AdjTriangles;

    auto getData = [&mesh] ( const WingedVertex& v
                           , glm::vec3& position, glm::vec3& delta, AdjTriangles& adjTriangles ) 
    {
      position = v.position (mesh);
      delta    = glm::vec3 (0.0f);
      adjTriangles.clear ();

      AdjFaces     adjFaces (v.adjacentFaces ());
      unsigned int valence  (0);

      for (auto it = adjFaces.begin (); it != adjFaces.end (); ++it) {
        PrimTriangle triangle = (*it).triangle (mesh);

        if (triangle.isDegenerated () == false) {
          adjTriangles.push_back (triangle);
        }
        delta += it.edge ()->otherVertexRef (v).position (mesh) - position;
        valence++;
      }
      delta /= float (valence);
    };

    auto getInterpolatedNormal = [] (const AdjTriangles& triangles) -> glm::vec3 {
      glm::vec3 normal = glm::vec3 (0.0f);

      for (const PrimTriangle& t : triangles) {
        assert (t.isDegenerated () == false);
        normal += t.normal ();
      }
      return normal / float (triangles.size ());
    };

    std::vector <glm::vec3> newPositions;

    for (WingedVertex* v : vertices) {
      glm::vec3    position;
      glm::vec3    delta;
      AdjTriangles adjTriangles;

      getData (*v, position, delta, adjTriangles);

      const glm::vec3 normal (getInterpolatedNormal (adjTriangles));
      const float     dot    (glm::dot (normal, delta));
      const glm::vec3 newP   ((position + delta) - (normal * dot));
      const PrimRay   ray    (true, newP, normal);

      bool intersected (false);
      for (const PrimTriangle& t : adjTriangles) {
        glm::vec3 intersection;
        if (IntersectionUtil::intersects (ray, t, &intersection)) {
          newPositions.push_back (intersection);
          intersected = true;
          break;
        }
      }
      if (intersected == false) {
        newPositions.push_back (newP);
      }
    }

    auto posIt = newPositions.begin ();
    for (WingedVertex* v : vertices) {
      assert (posIt != newPositions.end ());

      mesh.setVertex (v->index (), *posIt);
      ++posIt;
    }
  }
};

DELEGATE_BIG3   (ActionSmooth)
DELEGATE4       (void, ActionSmooth, run, WingedMesh&, const VertexPtrSet&, unsigned int, AffectedFaces&)
DELEGATE1_CONST (void, ActionSmooth, runUndo, WingedMesh&)
DELEGATE1_CONST (void, ActionSmooth, runRedo, WingedMesh&)

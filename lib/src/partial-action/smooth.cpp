#include <glm/glm.hpp>
#include "adjacent-iterator.hpp"
#include "affected-faces.hpp"
#include "intersection.hpp"
#include "partial-action/smooth.hpp"
#include "primitive/ray.hpp"
#include "primitive/triangle.hpp"
#include "winged/edge.hpp"
#include "winged/face.hpp"
#include "winged/mesh.hpp"
#include "winged/vertex.hpp"

namespace {
  void iteration (WingedMesh& mesh, const VertexPtrSet& vertices) {
    typedef std::vector <PrimTriangle> AdjTriangles;

    auto getData = [&mesh] ( const WingedVertex& v
                           , glm::vec3& position, glm::vec3& delta, AdjTriangles& adjTriangles ) 
    {
      assert (adjTriangles.empty ());

      position = v.position (mesh);
      delta    = glm::vec3 (0.0f);

      AdjFaces     adjFaces (v.adjacentFaces ());
      unsigned int valence  (0);

      for (auto it = adjFaces.begin (); it != adjFaces.end (); ++it) {
        adjTriangles.push_back (it->triangle (mesh));
        delta += it.edge ()->otherVertexRef (v).position (mesh) - position;
        valence++;
      }
      delta /= float (valence);
    };

    auto getInterpolatedNormal = [] (const AdjTriangles& triangles) -> glm::vec3 {
      glm::vec3    normal = glm::vec3 (0.0f);
      unsigned int n      = 0;

      for (const PrimTriangle& t : triangles) {
        if (t.isDegenerated () == false) {
          normal += t.normal ();
          n++;
        }
      }
      return n == 0 ? glm::vec3 (0.0f) : normal / float (n);
    };

    std::vector <glm::vec3> newPositions;
    newPositions.reserve (vertices.size ());

    for (WingedVertex* v : vertices) {
      glm::vec3    position;
      glm::vec3    delta;
      AdjTriangles adjTriangles;

      getData (*v, position, delta, adjTriangles);

      const glm::vec3 normal (getInterpolatedNormal (adjTriangles));

      if (normal == glm::vec3 (0.0f)) {
        newPositions.push_back (position);
      }
      else {
        const float     dot  (glm::dot (normal, delta));
        const glm::vec3 newP ((position + delta) - (normal * dot));
        const PrimRay   ray  (true, newP, normal);

        bool intersected (false);
        for (const PrimTriangle& t : adjTriangles) {
          glm::vec3 intersection;

          if (t.isDegenerated ()) {
            continue;
          }
          else {
            if (IntersectionUtil::intersects (ray, t, &intersection)) {
              newPositions.push_back (intersection);
              intersected = true;
              break;
            }
          }
        }
        if (intersected == false) {
          newPositions.push_back (newP);
        }
      }
    }

    auto posIt = newPositions.begin ();
    for (WingedVertex* v : vertices) {
      assert (posIt != newPositions.end ());

      mesh.setVertex (v->index (), *posIt);
      ++posIt;
    }
  }
}

void PartialAction :: smooth ( WingedMesh& mesh, const VertexPtrSet& vertices
                             , unsigned int numIterations, AffectedFaces& affectedFaces ) 
{
  for (unsigned int i = 0; i < numIterations; i++) {
    iteration (mesh, vertices);
  }

  for (WingedVertex* v : vertices) {
    for (WingedFace& f : v->adjacentFaces ()) {
      affectedFaces.insert (f);
    }
  }
}

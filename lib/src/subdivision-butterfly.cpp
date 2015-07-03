/* This file is part of Dilay
 * Copyright © 2015 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <functional>
#include <glm/glm.hpp>
#include <vector>
#include "adjacent-iterator.hpp"
#include "subdivision-butterfly.hpp"
#include "winged/edge.hpp"
#include "winged/face.hpp"
#include "winged/mesh.hpp"
#include "winged/vertex.hpp"

namespace {
  typedef std::vector <glm::vec3> Adjacents;

  glm::vec3 subdivK6 (const Adjacents& a1, const Adjacents& a2) {

    return (0.5f    * a1[0]) + (0.5f    * a2[0])
         + (0.125f  * a1[1]) + (0.125f  * a2[1])
         - (0.0625f * a1[2]) - (0.0625f * a2[2])
         - (0.0625f * a1[4]) - (0.0625f * a2[4]);
  }

  glm::vec3 subdivK (const glm::vec3& center, const Adjacents& a) {
    glm::vec3 v (0.0f,0.0f,0.0f);

    if (a.size () == 3) {
      v = (float (5/12) * (a[0] - center))
        - (float (1/12) * (a[1] - center))
        - (float (1/12) * (a[2] - center));
    }
    else if (a.size () == 4) {
      v = (0.375f * (a[0] - center))
        - (0.125f * (a[2] - center));
    }
    else {
      float K = float (a.size ());

      for (unsigned int i = 0; i < a.size (); i++) {
        float j   = float (i);
        float s_j = ( 0.25f 
                    +         cos ( 2.0f * M_PI * j / K ) 
                    + (0.5f * cos ( 4.0f * M_PI * j / K ))
                    ) / K;

        v = v + (s_j * (a[i] - center));
      }
    }
    return v + center;
  }

  glm::vec3 subdivExtraordinary (const Adjacents& a1, const Adjacents& a2) {

    return 0.5f * (subdivK (a2[0], a1) + subdivK (a1[0], a2));
  }

  glm::vec3 subdivide ( const glm::vec3& v1, const Adjacents& a1
                      , const glm::vec3& v2, const Adjacents& a2) {
    if (a1.size () == 6 && a2.size () == 6)
      return subdivK6 (a1,a2);
    else if (a1.size () == 6 && a2.size () != 6)
      return subdivK (v2, a2);
    else if (a1.size () != 6 && a2.size () == 6)
      return subdivK (v1, a1);
    else {
      return subdivExtraordinary (a1,a2);
    }
  }

  Adjacents adjacents (const WingedMesh& mesh, WingedEdge& edge, const WingedVertex& vertex) {
    const float edgeLength = glm::length (edge.vector (mesh));

    std::function < glm::vec3 (const WingedEdge&, const WingedVertex&, float) > traverse =
      [&mesh, &traverse, edgeLength] 
      (const WingedEdge& e, const WingedVertex& o, float oLength) -> glm::vec3 {
        WingedEdge* sibling = e.adjacentSibling (mesh, o);

        if (sibling) {
          const float sLength = oLength + glm::length (sibling->vector (mesh));
          if (glm::abs (edgeLength - oLength) < glm::abs (edgeLength - sLength) ) {
            return o.position (mesh);
          }
          else {
            return traverse (*sibling, sibling->otherVertexRef (o), sLength);
          }
        }
        else {
          return o.position (mesh);
        }
    };

    Adjacents adjacents;
    for (WingedEdge& e : vertex.adjacentEdges (edge)) {
      WingedVertex& a = e.otherVertexRef (vertex);

      adjacents.push_back (traverse (e, a, glm::length (e.vector (mesh))));
    }
    return adjacents;
  }
};

glm::vec3 SubdivisionButterfly::subdivideEdge (const WingedMesh& mesh, WingedEdge& edge) {
  WingedVertex& v1 = edge.vertex1Ref ();
  WingedVertex& v2 = edge.vertex2Ref ();
  Adjacents     a1 = adjacents (mesh, edge, v1);
  Adjacents     a2 = adjacents (mesh, edge, v2);

  return subdivide (v1.position (mesh), a1, v2.position (mesh), a2);
}

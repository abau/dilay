#define  _USE_MATH_DEFINES
#include <cmath>
#include "subdivision-butterfly.hpp"

typedef std::vector <WingedVertex*> Adjacents;

glm::vec3 subdivK6            (WingedMesh&, const Adjacents&, const Adjacents&);
glm::vec3 subdivK             (WingedMesh&, const Adjacents&);
glm::vec3 subdivExtraordinary (WingedMesh&, const Adjacents&, const Adjacents&);

NewFaces SubdivButterfly :: subdiv (WingedMesh& mesh, LinkedEdge& linkedEdge) {
  WingedEdge&   edge  = linkedEdge.data ();
  WingedVertex& v1    = edge.vertex1 ()->data ();
  WingedVertex& v2    = edge.vertex1 ()->data ();
  Adjacents     a1    = v1.adjacentVertices (edge);
  Adjacents     a2    = v2.adjacentVertices (edge);
  glm::vec3     v;

  if (a1.size () == 6 && a2.size () == 6)
    v = subdivK6 (mesh,a1,a2);
  else if (a1.size () == 6 && a2.size () < 6)
    v = subdivK (mesh,a2);
  else if (a1.size () < 6 && a2.size () == 6)
    v = subdivK (mesh,a1);
  else 
    v = subdivExtraordinary (mesh,a1,a2);

  return SubdivUtil :: splitEdge (mesh,linkedEdge,v);
}

glm::vec3 subdivK6 (WingedMesh& mesh, const Adjacents& a1, const Adjacents& a2) {

  return (0.5f    * a1[0]->vertex (mesh))
       + (0.5f    * a2[0]->vertex (mesh))
       + (0.125f  * a1[1]->vertex (mesh))
       + (0.125f  * a2[1]->vertex (mesh))
       - (0.0625f * a1[2]->vertex (mesh))
       - (0.0625f * a1[2]->vertex (mesh))
       - (0.0625f * a2[4]->vertex (mesh))
       - (0.0625f * a2[4]->vertex (mesh));
}

glm::vec3 subdivK (WingedMesh& mesh, const Adjacents& a) {
  glm::vec3 v (0.0f,0.0f,0.0f);

  if (a.size () == 3) {
    v = (float (5/12) * a[0]->vertex (mesh))
      - (float (1/12) * a[1]->vertex (mesh))
      - (float (1/12) * a[2]->vertex (mesh));
  }
  else if (a.size () == 4) {
    v = (0.375f * a[0]->vertex (mesh))
      - (0.125f * a[2]->vertex (mesh));
  }
  else {
    float     K = float (a.size ());

    for (unsigned int i = 0; i < a.size (); i++) {
      float j   = float (i);
      float s_j = ( 0.25f 
                  +         cos ( (2.0f * M_PI * j) / K ) 
                  + (0.5f * cos ( (4.0f * M_PI * j) / K ))
                  ) / K;

      v = v + (s_j * a[i]->vertex (mesh));
    }
  }
  return v;
}

glm::vec3 subdivExtraordinary ( WingedMesh& mesh
                              , const Adjacents& a1, const Adjacents& a2) {

  return 0.5f * (subdivK (mesh,a1) + subdivK (mesh,a2));
}

#define  _USE_MATH_DEFINES
#include <cmath>
#include "subdivision-butterfly.hpp"
#include "winged-edge-iterator.hpp"

typedef std::vector <LinkedVertex>   Adjacents;

void SubdivButterfly :: subdiv (WingedMesh& mesh) {
  typedef std::pair  <LinkedEdge, glm::vec3> NewVertex;
  typedef std::list  <NewVertex>             NewVertices;

  NewVertices newVertices;

  for (EDGE_ITERATOR(eIt,mesh)) {
    glm::vec3 v = SubdivButterfly :: subdiv (mesh, eIt);
    newVertices.push_back ( NewVertex (eIt, v) );
  }

  for (NewVertex& nV : newVertices) {
    SubdivUtil :: insertVertex (mesh, nV.first, nV.second);
  }
  
  for (FACE_REVERSE_ITERATOR (fIt,mesh)) {
    SubdivUtil :: triangulate6Gon (mesh,fIt);
  }

  /*
  for (FACE_ITERATOR(f,mesh)) {
    if (f->id () == 33) {
      for (WINGED_EDGE_ITERATOR(eIt,*f)) {
        glm::vec3 v = SubdivButterfly :: subdiv (mesh,eIt.edge ());
        newVertices.push_back ( NewVertex (eIt.edge (), v) );
      }
      break;
    }
  }

  for (NewVertex& nV : newVertices) {
    SubdivUtil :: insertVertex (mesh, nV.first, nV.second);
  }
  
  for (FACE_REVERSE_ITERATOR (fIt,mesh)) {
    if (fIt->id () == 33) {
      SubdivUtil :: triangulate6Gon (mesh,fIt);
      break;
    }
  }
  */
}

glm::vec3 subdivK6            (const WingedMesh&, const Adjacents&, const Adjacents&);
glm::vec3 subdivK             (const WingedMesh&, const glm::vec3&, const Adjacents&);
glm::vec3 subdivExtraordinary (const WingedMesh&, const Adjacents&, const Adjacents&);

glm::vec3 SubdivButterfly :: subdiv (const WingedMesh& mesh, LinkedEdge edge) {
  LinkedVertex  v1    = edge->vertex1 ();
  LinkedVertex  v2    = edge->vertex2 ();
  Adjacents     a1    = v1->adjacentVertices (edge);
  Adjacents     a2    = v2->adjacentVertices (edge);

  if (a1.size () == 6 && a2.size () == 6)
    return subdivK6 (mesh,a1,a2);
  else if (a1.size () == 6 && a2.size () < 6)
    return subdivK (mesh,v2->vertex (mesh), a2);
  else if (a1.size () < 6 && a2.size () == 6)
    return subdivK (mesh,v1->vertex (mesh), a1);
  else 
    return subdivExtraordinary (mesh,a1,a2);
}

glm::vec3 subdivK6 (const WingedMesh& mesh, const Adjacents& a1, const Adjacents& a2) {

  return (0.5f    * a1[0]->vertex (mesh))
       + (0.5f    * a2[0]->vertex (mesh))
       + (0.125f  * a1[1]->vertex (mesh))
       + (0.125f  * a2[1]->vertex (mesh))
       - (0.0625f * a1[2]->vertex (mesh))
       - (0.0625f * a2[2]->vertex (mesh))
       - (0.0625f * a1[4]->vertex (mesh))
       - (0.0625f * a2[4]->vertex (mesh));
}

glm::vec3 subdivK (const WingedMesh& mesh, const glm::vec3& center, const Adjacents& a) {
  glm::vec3 v (0.0f,0.0f,0.0f);

  if (a.size () == 3) {
    v = (float (5/12) * (a[0]->vertex (mesh) - center))
      - (float (1/12) * (a[1]->vertex (mesh) - center))
      - (float (1/12) * (a[2]->vertex (mesh) - center));
  }
  else if (a.size () == 4) {
    v = (0.375f * (a[0]->vertex (mesh) - center))
      - (0.125f * (a[2]->vertex (mesh) - center));
  }
  else {
    float     K = float (a.size ());

    for (unsigned int i = 0; i < a.size (); i++) {
      float j   = float (i);
      float s_j = ( 0.25f 
                  +         cos ( 2.0f * M_PI * j / K ) 
                  + (0.5f * cos ( 4.0f * M_PI * j / K ))
                  ) / K;

      v = v + (s_j * (a[i]->vertex (mesh) - center));
    }
  }
  return v + center;
}

glm::vec3 subdivExtraordinary ( const WingedMesh& mesh
                              , const Adjacents& a1, const Adjacents& a2) {

  return 0.5f * ( subdivK (mesh, a2[0]->vertex (mesh), a1) 
                + subdivK (mesh, a1[0]->vertex (mesh), a2)
                );
}

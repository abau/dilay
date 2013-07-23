#include <vector>
#include <list>
#include <unordered_map>
#include <glm/glm.hpp>
#include "subdivision-butterfly.hpp"
#include "winged-vertex.hpp"
#include "winged-edge.hpp"
#include "winged-face.hpp"
#include "winged-mesh.hpp"
#include "winged-util.hpp"
#include "adjacent-iterator.hpp"
#include "subdivision-util.hpp"
#include "maybe.hpp"

void subdivide (WingedMesh&, unsigned int, LinkedFace);

void SubdivButterfly :: subdivide (WingedMesh& mesh, LinkedFace face) {
  subdivide (mesh,face->level (),face);
}

bool       oneRingNeighbourhood (WingedMesh&, LinkedFace, FaceSet&);
bool       oneRingBorder        (WingedMesh&, unsigned int, FaceSet&, FaceSet&);
void       subdivideFaces       (WingedMesh&, FaceSet&, unsigned int);
void       refineBorder         (WingedMesh&, FaceSet&, unsigned int);

void subdivide (WingedMesh& mesh, unsigned int selectionLevel, LinkedFace selection) {
  if (selection->level () > selectionLevel)
    return;

  FaceSet neighbourhood, border;

  if (! oneRingNeighbourhood (mesh,selection,neighbourhood)) {
    return subdivide (mesh, selectionLevel, selection);
  }
  
  if (! oneRingBorder (mesh,selectionLevel,neighbourhood,border)) {
    return subdivide (mesh, selectionLevel, selection);
  }

  subdivideFaces (mesh,neighbourhood,selectionLevel);
  refineBorder   (mesh,border,selectionLevel);
}

void insertNeighbour (FaceSet&, LinkedFace);

bool oneRingNeighbourhood ( WingedMesh& mesh, LinkedFace selection
                          , FaceSet& neighbourhood) {
  neighbourhood.clear ();
  neighbourhood.insert (selection);
  unsigned int selectionLevel = selection->level ();

  for (auto vIt = selection->adjacentVertexIterator (true); vIt.isValid ()
                                                          ; vIt.next ()) {
    for (ADJACENT_FACE_ITERATOR (fIt, *vIt.element ())) {
      LinkedFace face = fIt.element ();
      unsigned int faceLevel = face->level ();
      if (faceLevel < selectionLevel) {
        subdivide (mesh, faceLevel, face);
        return false;
      }
      else if (faceLevel == selectionLevel) { 
        insertNeighbour (neighbourhood, face);
      }
    }
  }
  return true;
}

bool oneRingBorder ( WingedMesh& mesh, unsigned int selectionLevel
                   , FaceSet& neighbourhood, FaceSet& border) {
  FaceSet extendedNeighbourhood;

  std::function < bool (LinkedFace) > isNeighbour =
    [&neighbourhood,&extendedNeighbourhood] (LinkedFace face) {

      return (neighbourhood.count (face) > 0) 
          || (extendedNeighbourhood.count (face) > 0);
    };

  std::function < bool (LinkedFace) > hasAtLeast2AdjacentNeighbours =
    [&isNeighbour] (LinkedFace face) {
      unsigned int numNeighbours = 0;

      for (ADJACENT_FACE_ITERATOR (it, *face)) {
        if (isNeighbour (it.element ())) {
          numNeighbours++;
        }
      }
      return numNeighbours >= 2;
    };

  std::function < bool (LinkedFace) > checkNeighbour;

  std::function < bool (LinkedFace) > checkBorder = [ & ] (LinkedFace face) {
    if (! isNeighbour (face)) {
      unsigned int faceLevel = face->level ();

      if (faceLevel <  selectionLevel) {
        subdivide (mesh,faceLevel,face);
        return false;
      }
      else if (faceLevel == selectionLevel) {
        if (face->tEdge ().isDefined ()) {
          insertNeighbour (extendedNeighbourhood,face);
          border.erase    (face);
          return checkNeighbour (face);
        }
        else {
          if (hasAtLeast2AdjacentNeighbours (face)) {
            insertNeighbour (extendedNeighbourhood,face);
            border.erase    (face);
            return checkNeighbour (face);
          }
          else {
            border.insert (face);
            return true;
          }
        }
      }
    }
    return true;
  };

  checkNeighbour = [ & ] (LinkedFace neighbour) {
    for (auto it = neighbour->adjacentFaceIterator (true); it.isValid (); it.next ()) {
      if (! checkBorder (it.element ()))
        return false;
    }
    return true;
  };

  border.clear ();

  for (LinkedFace neighbour : neighbourhood) {
    if (! checkNeighbour (neighbour)) {
      return false;
    }
  }

  neighbourhood.insert (extendedNeighbourhood.begin (), extendedNeighbourhood.end ());
  return true;
}

void insertNeighbour (FaceSet& neighbourhood, LinkedFace neighbour) {
  neighbourhood.insert (neighbour);

  Maybe <LinkedEdge> tEdge = neighbour->tEdge ();
  if (tEdge.isDefined ()) {
    neighbourhood.insert (tEdge.data ()->otherFace (*neighbour));
  }
}

glm::vec3  subdivideEdge (const WingedMesh&, unsigned int, LinkedEdge);
void       deleteTEdges  (WingedMesh&, FaceSet&);

void subdivideFaces (WingedMesh& mesh, FaceSet& faces, unsigned int selectionLevel) {
  deleteTEdges (mesh,faces);

  for (LinkedFace face : faces) {
    for (auto it = face->adjacentEdgeIterator (); it.isValid (); ) {
      LinkedEdge edge = it.element ();
      assert (! edge->isTEdge ());
      it.next ();

      if (   edge->vertex1 ()->level () <= selectionLevel
          && edge->vertex2 ()->level () <= selectionLevel) {

        SubdivUtil :: insertVertex ( mesh, edge
                                   , subdivideEdge (mesh, selectionLevel, edge)
                                   );
      }
    }
  }
  FaceSet newFaces;
  for (LinkedFace face : faces) {
    LinkedFace realignedFace = SubdivUtil :: triangulate6Gon (mesh,face);

    newFaces.insert (realignedFace);

    for (ADJACENT_FACE_ITERATOR (it,*realignedFace)) {
      newFaces.insert (it.element ());
    }
  }
  faces = std::move (newFaces);
}

void refineBorder (WingedMesh& mesh, FaceSet& border, unsigned int selectionLevel) {
  deleteTEdges (mesh, border);

  for (LinkedFace face : border) {
    assert (face->level () == selectionLevel);
    assert (face->tEdge ().isUndefined ());
    SubdivUtil :: triangulateQuadAtHeighestLevelVertex (mesh, face);
  }
}

void deleteTEdges (WingedMesh& mesh, FaceSet& faces) {
  for (auto it = faces.begin (); it != faces.end (); ) {
    LinkedFace face = *it;
    ++it;

    Maybe <LinkedEdge> tEdge = face->tEdge ();
    if (tEdge.isDefined () && tEdge.data ()->isRightFace (*face)) {
      LinkedFace f = mesh.deleteEdge (tEdge.data ());
      faces.erase     (face);
      assert (f->tEdge ().isUndefined ());
    }
  }
}

typedef std::vector <LinkedVertex> Adjacents;
Adjacents adjacents           (const WingedMesh&, LinkedVertex, unsigned int, LinkedEdge);
glm::vec3 subdivK6            (const WingedMesh&, const Adjacents&, const Adjacents&);
glm::vec3 subdivK             (const WingedMesh&, const glm::vec3&, const Adjacents&);
glm::vec3 subdivExtraordinary (const WingedMesh&, const Adjacents&, const Adjacents&);

glm::vec3 subdivideEdge ( const WingedMesh& mesh, unsigned int selectionLevel
                        , LinkedEdge edge) {
  LinkedVertex  v1    = edge->vertex1 ();
  LinkedVertex  v2    = edge->vertex2 ();
  Adjacents     a1    = adjacents     (mesh,v1,selectionLevel,edge);
  Adjacents     a2    = adjacents     (mesh,v2,selectionLevel,edge);

  if (a1.size () == 6 && a2.size () == 6)
    return subdivK6 (mesh,a1,a2);
  else if (a1.size () == 6 && a2.size () < 6)
    return subdivK (mesh,v2->vertex (mesh), a2);
  else if (a1.size () < 6 && a2.size () == 6)
    return subdivK (mesh,v1->vertex (mesh), a1);
  else 
    return subdivExtraordinary (mesh,a1,a2);
}

Maybe <LinkedVertex> compatibleLevelAdjacent ( const WingedMesh&, unsigned int
                                             , LinkedEdge, LinkedVertex);

Adjacents adjacents ( const WingedMesh& mesh, LinkedVertex v
                    , unsigned int selectionLevel, LinkedEdge e) {
  Adjacents adjacents;
  for (auto it = v->adjacentEdgeIterator (e); it.isValid (); it.next ()) {
    if (! it.element ()->isTEdge ()) {
      Maybe <LinkedVertex> a = compatibleLevelAdjacent (mesh,selectionLevel,it.element (),v);
      if (a.isDefined ())
        adjacents.push_back (a.data ());
    }
  }
  return adjacents;
}

Maybe <LinkedVertex> compatibleLevelAdjacent ( const WingedMesh& mesh
                                             , unsigned int level
                                             , LinkedEdge e, LinkedVertex v) {
  LinkedVertex other = e->otherVertex (*v);
  if (other->level () <= level) 
    return Maybe <LinkedVertex> (other);
  else {
    Maybe <LinkedEdge> sibling = e->adjacentSibling (*other);
    if (sibling.isUndefined ()) {
      return Maybe <LinkedVertex> ();
    }
    else {
      return compatibleLevelAdjacent (mesh, level, sibling.data (), other);
    }
  }
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

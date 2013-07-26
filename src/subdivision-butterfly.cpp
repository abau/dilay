#include <vector>
#include <list>
#include <unordered_map>
#include <functional>
#include <glm/glm.hpp>
#include <limits>
#include "subdivision-butterfly.hpp"
#include "winged-vertex.hpp"
#include "winged-edge.hpp"
#include "winged-face.hpp"
#include "winged-mesh.hpp"
#include "winged-util.hpp"
#include "adjacent-iterator.hpp"
#include "subdivision-util.hpp"

#include <iostream> // delete this

typedef std::unordered_set <WingedFace*> FaceSet;

void subdivide (WingedMesh&, unsigned int, WingedFace&);

void SubdivButterfly :: subdivide (WingedMesh& mesh, WingedFace& face) {
  subdivide (mesh,face.level (),face);
}

/*
void SubdivButterfly :: subdivide ( WingedMesh& mesh
                                  , std::list <WingedFace*>& faces) {
  if (faces.empty ()) return;

  unsigned int minLevel = std::numeric_limits <unsigned int>::max ();

  // search minimal level
  for (WingedFace* face : faces) {
    unsigned int level = face->level ();
    minLevel = level < minLevel ? level : minLevel;
  }

  // don't subdivide faces of higher levels
  std::cout << "######################################\n";
  for (auto it = faces.begin (); it != faces.end (); ) {
    LinkedFace         face  = *it;
    unsigned int       level = face->level ();
    Maybe <LinkedEdge> tEdge = face->tEdge ();
    if (level == minLevel && (  tEdge.isUndefined () 
                             || tEdge.data ()->isLeftFace (*face))) {
      std::cout << "subdividing " << face->id () << std::endl;
      ++it;
    }
    else {
      faces.erase (it++);
    }
  }
  std::cout << "######################################\n";

  std::cout << "--------------------------------------\n";
  std::cout << "subdividing " << faces.size () << " faces of level " << minLevel << std::endl;

  for (auto it = faces.begin (); it != faces.end (); ++it) {
    LinkedFace f = *it;
    std::cout << "subdividing face " << f->id () << std::endl;
    subdivide (mesh,minLevel,f);

    for (auto it2 = it; it2 != faces.end (); ++it2) {
      LinkedFace f2 = *it2;
      std::cout << "remaining face " << f2->id () 
                << " of level "<< f2->level () 
                << std::endl;
    }
  }
  std::cout << "--------------------------------------\n";
}
*/

bool       oneRingNeighbourhood (WingedMesh&, WingedFace&, FaceSet&);
bool       oneRingBorder        (WingedMesh&, unsigned int, FaceSet&, FaceSet&);
void       subdivideFaces       (WingedMesh&, FaceSet&, unsigned int);
void       refineBorder         (WingedMesh&, FaceSet&, unsigned int);

void subdivide (WingedMesh& mesh, unsigned int selectionLevel, WingedFace& selection) {
  if (selection.level () > selectionLevel)
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

void insertNeighbour (FaceSet&, WingedFace&);

bool oneRingNeighbourhood ( WingedMesh& mesh, WingedFace& selection
                          , FaceSet& neighbourhood) {
  neighbourhood.clear  ();
  neighbourhood.insert (&selection);
  unsigned int selectionLevel = selection.level ();

  for (auto vIt = selection.adjacentVertexIterator (true); vIt.isValid ()
                                                         ; vIt.next ()) {
    for (ADJACENT_FACE_ITERATOR (fIt, vIt.element ())) {
      WingedFace& face = fIt.element ();
      unsigned int faceLevel = face.level ();
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

  std::function < bool (WingedFace&) > isNeighbour =
    [&neighbourhood,&extendedNeighbourhood] (WingedFace& face) {

      return (neighbourhood        .count (&face) > 0) 
          || (extendedNeighbourhood.count (&face) > 0);
    };

  std::function < bool (WingedFace&) > hasAtLeast2AdjacentNeighbours =
    [&isNeighbour] (WingedFace& face) {
      unsigned int numNeighbours = 0;

      for (ADJACENT_FACE_ITERATOR (it, face)) {
        if (isNeighbour (it.element ())) {
          numNeighbours++;
        }
      }
      return numNeighbours >= 2;
    };

  std::function < bool (WingedFace&) > checkNeighbour;

  std::function < bool (WingedFace&) > checkBorder = [ & ] (WingedFace& face) {
    if (! isNeighbour (face)) {
      unsigned int faceLevel = face.level ();

      if (faceLevel <  selectionLevel) {
        subdivide (mesh,faceLevel,face);
        return false;
      }
      else if (faceLevel == selectionLevel) {
        if (face.tEdge ()) {
          insertNeighbour (extendedNeighbourhood,face);
          border.erase    (&face);
          return checkNeighbour (face);
        }
        else {
          if (hasAtLeast2AdjacentNeighbours (face)) {
            insertNeighbour (extendedNeighbourhood,face);
            border.erase    (&face);
            return checkNeighbour (face);
          }
          else {
            border.insert (&face);
            return true;
          }
        }
      }
    }
    return true;
  };

  checkNeighbour = [ & ] (WingedFace& neighbour) {
    for (auto it = neighbour.adjacentFaceIterator (true); it.isValid (); it.next ()) {
      if (! checkBorder (it.element ()))
        return false;
    }
    return true;
  };

  border.clear ();

  for (WingedFace* neighbour : neighbourhood) {
    if (! checkNeighbour (*neighbour)) {
      return false;
    }
  }

  neighbourhood.insert (extendedNeighbourhood.begin (), extendedNeighbourhood.end ());
  return true;
}

void insertNeighbour (FaceSet& neighbourhood, WingedFace& neighbour) {
  neighbourhood.insert (&neighbour);

  WingedEdge* tEdge = neighbour.tEdge ();
  if (tEdge) {
    neighbourhood.insert (tEdge->otherFace (neighbour));
  }
}

glm::vec3  subdivideEdge (const WingedMesh&, unsigned int, WingedEdge&);
void       deleteTEdges  (WingedMesh&, FaceSet&);

void subdivideFaces (WingedMesh& mesh, FaceSet& faces, unsigned int selectionLevel) {
  deleteTEdges (mesh,faces);

  for (WingedFace* face : faces) {
    for (auto it = face->adjacentEdgeIterator (); it.isValid (); ) {
      WingedEdge& edge = it.element ();
      assert (! edge.isTEdge ());
      it.next ();

      if (   edge.vertex1Ref ().level () <= selectionLevel
          && edge.vertex2Ref ().level () <= selectionLevel) {

        SubdivUtil :: insertVertex ( mesh, edge
                                   , subdivideEdge (mesh, selectionLevel, edge)
                                   );
      }
    }
  }
  FaceSet newFaces;
  for (WingedFace* face : faces) {
    WingedFace& realignedFace = SubdivUtil :: triangulate6Gon (mesh,*face);

    newFaces.insert (&realignedFace);

    for (ADJACENT_FACE_ITERATOR (it,realignedFace)) {
      newFaces.insert (&it.element ());
    }
  }
  faces = std::move (newFaces);
}

void refineBorder (WingedMesh& mesh, FaceSet& border, unsigned int selectionLevel) {
  deleteTEdges (mesh, border);

  for (WingedFace* face : border) {
    assert (face->level () == selectionLevel);
    assert (face->tEdge () == nullptr);
    SubdivUtil :: triangulateQuadAtHeighestLevelVertex (mesh,*face);
  }
}

void deleteTEdges (WingedMesh& mesh, FaceSet& faces) {
  for (auto it = faces.begin (); it != faces.end (); ) {
    auto        faceIt = it;
    WingedFace* face   = *it;
    ++it;

    WingedEdge* tEdge = face->tEdge ();
    if (tEdge && tEdge->isRightFace (*face)) {
      WingedFace& f = mesh.deleteEdge (*tEdge);
      faces.erase (faceIt);
      assert (f.tEdge () == nullptr);
    }
  }
}

typedef std::vector <WingedVertex*> Adjacents;
Adjacents adjacents           (const WingedMesh&, WingedVertex&, unsigned int, WingedEdge&);
glm::vec3 subdivK6            (const WingedMesh&, const Adjacents&, const Adjacents&);
glm::vec3 subdivK             (const WingedMesh&, const glm::vec3&, const Adjacents&);
glm::vec3 subdivExtraordinary (const WingedMesh&, const Adjacents&, const Adjacents&);

glm::vec3 subdivideEdge ( const WingedMesh& mesh, unsigned int selectionLevel
                        , WingedEdge& edge) {
  WingedVertex& v1    = edge.vertex1Ref ();
  WingedVertex& v2    = edge.vertex2Ref ();
  Adjacents     a1    = adjacents     (mesh,v1,selectionLevel,edge);
  Adjacents     a2    = adjacents     (mesh,v2,selectionLevel,edge);

  if (a1.size () == 6 && a2.size () == 6)
    return subdivK6 (mesh,a1,a2);
  else if (a1.size () == 6 && a2.size () < 6)
    return subdivK (mesh,v2.vertex (mesh), a2);
  else if (a1.size () < 6 && a2.size () == 6)
    return subdivK (mesh,v1.vertex (mesh), a1);
  else 
    return subdivExtraordinary (mesh,a1,a2);
}

WingedVertex* compatibleLevelAdjacent ( const WingedMesh&, unsigned int
                                      , WingedEdge&, WingedVertex&);

Adjacents adjacents ( const WingedMesh& mesh, WingedVertex& v
                    , unsigned int selectionLevel, WingedEdge& e) {
  Adjacents adjacents;
  for (auto it = v.adjacentEdgeIterator (e); it.isValid (); it.next ()) {
    if (! it.element ().isTEdge ()) {
      WingedVertex* a = compatibleLevelAdjacent (mesh,selectionLevel,it.element (),v);
      if (a)
        adjacents.push_back (a);
    }
  }
  return adjacents;
}

WingedVertex* compatibleLevelAdjacent ( const WingedMesh& mesh
                                             , unsigned int level
                                             , WingedEdge& e, WingedVertex& v) {
  WingedVertex& other = e.otherVertexRef (v);
  if (other.level () <= level) 
    return &other;
  else {
    WingedEdge* sibling = e.adjacentSibling (other);
    if (sibling) {
      return compatibleLevelAdjacent (mesh, level, *sibling, other);
    }
    else {
      return nullptr;
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

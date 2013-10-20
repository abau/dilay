#include <vector>
#include <functional>
#include <glm/glm.hpp>
#include "subdivision-butterfly.hpp"
#include "winged-vertex.hpp"
#include "winged-edge.hpp"
#include "winged-face.hpp"
#include "winged-mesh.hpp"
#include "adjacent-iterator.hpp"

/*
typedef std::unordered_set <WingedFace*> FaceSet;

void subdivide (WingedMesh&, unsigned int, WingedFace&);

void SubdivButterfly :: subdivide (WingedMesh& mesh, WingedEdge& edge, float maxLength) {
  if (edge.lengthSqr (mesh) >= maxLength * maxLength) {
    unsigned int leftLevel  = edge.leftFace  ()->level ();
    unsigned int rightLevel = edge.rightFace ()->level ();
    subdivide (mesh, leftLevel,  *edge.leftFace ());
    subdivide (mesh, rightLevel, *edge.rightFace ());
  }
}

void SubdivButterfly :: subdivide (WingedMesh& mesh, WingedFace& face) {
  subdivide (mesh,face.level (),face);
}

void SubdivButterfly :: subdivide (WingedMesh& mesh, std::list <Id>& ids) {
  if (ids.empty ()) return;

  unsigned int minLevel = std::numeric_limits <unsigned int>::max ();

  // search minimal level
  for (auto& id : ids) {
    unsigned int level = mesh.octree ().faceRef (id).level ();
    minLevel = level < minLevel ? level : minLevel;
  }

  // don't subdivide faces of higher levels
  for (auto it = ids.begin (); it != ids.end (); ) {
    WingedFace&  face  = mesh.octree ().faceRef (*it);
    unsigned int level = face.level ();
    WingedEdge*  tEdge = face.tEdge ();
    if (level == minLevel && (tEdge == nullptr || tEdge->isLeftFace (face))) {
      ++it;
    }
    else {
      ids.erase (it++);
    }
  }

  for (auto it = ids.begin (); it != ids.end (); ++it) {
    WingedFace* face = mesh.octree ().face (*it);
    if (face)
      subdivide (mesh,minLevel,*face);
  }
}

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
  for (WingedFace* face : faces) {
    SubdivUtil :: triangulate6Gon (mesh,*face);
  }
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
      WingedFace& leftFace = tEdge->leftFaceRef ();
      mesh.deleteEdge (*tEdge);
      faces.erase (faceIt);
      assert (leftFace.tEdge () == nullptr);
    }
  }
}
*/

typedef std::vector <glm::vec3> Adjacents;
glm::vec3 subdivide (const glm::vec3&, const Adjacents&, const glm::vec3&, const Adjacents&);
Adjacents adjacents (unsigned int, const WingedMesh&, WingedEdge&, const WingedVertex&);

glm::vec3 SubdivisionButterfly::subdivideEdge (const WingedMesh& mesh, WingedEdge& edge) {
  WingedVertex& v1            = edge.vertex1Ref ();
  WingedVertex& v2            = edge.vertex2Ref ();
  unsigned int  maxLevel      = glm::max <unsigned int> (v1.level (), v2.level ());
  Adjacents     a1            = adjacents (maxLevel, mesh, edge, v1);
  Adjacents     a2            = adjacents (maxLevel, mesh, edge, v2);

  return subdivide (v1.vertex (mesh), a1, v2.vertex (mesh), a2);
}

glm::vec3 subdivK6            (const Adjacents&, const Adjacents&);
glm::vec3 subdivK             (const glm::vec3&, const Adjacents&);
glm::vec3 subdivExtraordinary (const Adjacents&, const Adjacents&);

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

Adjacents adjacents ( unsigned int maxLevel, const WingedMesh& mesh
                    , WingedEdge& edge, const WingedVertex& vertex) {
  // traverses edge's siblings until it finds a vertex with level <= `maxLevel`
  std::function < glm::vec3 (const WingedEdge&, const WingedVertex&) > traverse =
    [maxLevel, &mesh, &traverse] (const WingedEdge& e, const WingedVertex& o) {

      if (o.level () <= maxLevel) {
        return o.vertex (mesh);
      }
      else {
        WingedEdge* sibling = e.adjacentSibling (o);
        assert (sibling);

        return traverse (*sibling, sibling->otherVertexRef (o));
      }
  };

  Adjacents adjacents;
  for (auto it = vertex.adjacentEdgeIterator (edge,true); it.isValid (); it.next ()) {
    WingedEdge&   e = it.element ();
    WingedVertex& a = e.otherVertexRef (vertex);
    adjacents.push_back (traverse (e,a));
  }
  return adjacents;
}

/*
WingedVertex* compatibleLevelAdjacent ( const WingedMesh&, unsigned int
                                      , WingedEdge&, WingedVertex&);

Adjacents adjacents ( const WingedMesh& mesh, WingedVertex& v
                    , unsigned int selectionLevel, WingedEdge& e) {
  Adjacents adjacents;
  for (auto it = v.adjacentEdgeIterator (e); it.isValid (); it.next ()) {
    if (! it.element ().isTEdge ()) {
      WingedVertex* a = compatibleLevelAdjacent (mesh,selectionLevel,it.element (),v);
      if (a)
        adjacents.push_back (a->vertex (mesh));
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
*/

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

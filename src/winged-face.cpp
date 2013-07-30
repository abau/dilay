#include <limits>
#include <algorithm>
#include <glm/glm.hpp>
#include "winged-vertex.hpp"
#include "winged-edge.hpp"
#include "winged-face.hpp"
#include "winged-mesh.hpp"
#include "adjacent-iterator.hpp"
#include "triangle.hpp"
#include "octree.hpp"

WingedFace :: WingedFace (WingedEdge* e, const Id& id) 
  : _id (id), _edge (e), _octreeNode (nullptr) {}

void WingedFace :: addIndices (WingedMesh& mesh) {
  for (ADJACENT_VERTEX_ITERATOR (it,*this)) {
    it.element ().addIndex (mesh);
  }
}

Triangle WingedFace :: triangle (const WingedMesh& mesh) const {
  WingedEdge& e1 = *this->_edge;
  WingedEdge& e2 = *e1.successor (*this);

  return Triangle ( e1.firstVertex  (*this)->vertex (mesh)
                  , e1.secondVertex (*this)->vertex (mesh)
                  , e2.secondVertex (*this)->vertex (mesh)
  );
}

unsigned int WingedFace :: numEdges () const {
  unsigned int i = 0;

  for (ADJACENT_EDGE_ITERATOR(_,*this)) {
    i++;
  }
  return i;
}

glm::vec3 WingedFace :: normal (const WingedMesh& mesh) const {
  glm::vec3 v1 = this->_edge->vertex (*this,0)->vertex (mesh);
  glm::vec3 v2 = this->_edge->vertex (*this,1)->vertex (mesh);
  glm::vec3 v3 = this->_edge->vertex (*this,2)->vertex (mesh);

  return glm::normalize (glm::cross (v2-v1, v3-v2));
}

WingedEdge* WingedFace :: adjacent (const WingedVertex& vertex) const {
  for (ADJACENT_EDGE_ITERATOR (it,*this)) {
    if (it.element ().isAdjacent (vertex))
      return &it.element ();
  }
  assert (false);
}

WingedEdge* WingedFace :: longestEdge (const WingedMesh& mesh) const {
  WingedEdge* longest   = this->edge ();
  float       maxLength = 0.0f;

  for (ADJACENT_EDGE_ITERATOR (it,*this)) {
    float length = it.element ().length (mesh);
    if (length > maxLength) {
      maxLength = length;
      longest   = &it.element ();
    }
  }
  return longest;
}

WingedVertex* WingedFace :: tVertex () const {
  for (ADJACENT_VERTEX_ITERATOR (it,*this)) {
    if (it.element ().tEdge ())
      return &it.element ();
  }
  return nullptr;
}

WingedEdge* WingedFace :: tEdge () const {
  for (ADJACENT_EDGE_ITERATOR (it,*this)) {
    if (it.element ().isTEdge ())
      return &it.element ();
  }
  return nullptr;
}

unsigned int WingedFace :: level () const {
  std::function <unsigned int (unsigned int)> levelMin;
  levelMin = [&levelMin,this] (unsigned int min) {
    unsigned int l = std::numeric_limits <unsigned int>::max ();
    for (ADJACENT_EDGE_ITERATOR (it,*this)) {
      if (it.element ().isTEdge ())
        return it.element ().vertex1Ref ().level () - 1;
      if (it.element ().vertex1Ref ().level () >= min)
        l = std::min <unsigned int> (l, it.element ().vertex1Ref ().level ());
      if (it.element ().vertex2Ref ().level () >= min)
        l = std::min <unsigned int> (l, it.element ().vertex2Ref ().level ());
    }
    unsigned int count = 0;
    for (ADJACENT_VERTEX_ITERATOR (it,*this)) {
      if (it.element ().level () == l)
        count++;
    }
    if (count == 1)
      return levelMin (l+1);
    else
      return l;
  };
  return levelMin (0);
}

WingedVertex* WingedFace :: highestLevelVertex () const {
  WingedVertex* v = nullptr;
  for (ADJACENT_VERTEX_ITERATOR (it,*this)) {
    if (v == nullptr || it.element ().level () > v->level ()) {
      v = &it.element ();
    }
  }
  return v;
}

AdjacentEdgeIterator WingedFace :: adjacentEdgeIterator (bool skipT) const {
  return AdjacentEdgeIterator (*this,skipT);
}
AdjacentVertexIterator WingedFace :: adjacentVertexIterator (bool skipT) const {
  return AdjacentVertexIterator (*this,skipT);
}
AdjacentFaceIterator WingedFace :: adjacentFaceIterator (bool skipT) const {
  return AdjacentFaceIterator (*this,skipT);
}
AdjacentEdgeIterator WingedFace :: adjacentEdgeIterator ( WingedEdge& e
                                                        , bool skipT) const {
  return AdjacentEdgeIterator (*this,e,skipT);
}
AdjacentVertexIterator WingedFace :: adjacentVertexIterator ( WingedEdge& e
                                                            , bool skipT) const {
  return AdjacentVertexIterator (*this,e,skipT);
}
AdjacentFaceIterator WingedFace :: adjacentFaceIterator ( WingedEdge& e
                                                        , bool skipT) const {
  return AdjacentFaceIterator (*this,e,skipT);
}

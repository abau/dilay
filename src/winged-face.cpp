#include <limits>
#include <algorithm>
#include <glm/glm.hpp>
#include "winged-vertex.hpp"
#include "winged-edge.hpp"
#include "winged-face.hpp"
#include "winged-mesh.hpp"
#include "adjacent-iterator.hpp"
#include "triangle.hpp"
#include "maybe.hpp"

WingedFace :: WingedFace () {}

WingedFace :: WingedFace (LinkedEdge e) 
  : _edge (e){}

void WingedFace :: setEdge (LinkedEdge e) {
  this->_edge = e;
}

void WingedFace :: addIndices (WingedMesh& mesh) {
  for (ADJACENT_VERTEX_ITERATOR (it,*this)) {
    it.vertex ()->addIndex (mesh);
  }
}

void WingedFace :: triangle (const WingedMesh& mesh, Triangle& triangle) const {
  WingedEdge& e1 = *this->_edge;
  WingedEdge& e2 = *e1.successor (*this);

  triangle.vertex1 (e1.firstVertex  (*this)->vertex (mesh));
  triangle.vertex2 (e1.secondVertex (*this)->vertex (mesh));
  triangle.vertex3 (e2.secondVertex (*this)->vertex (mesh));
}

Triangle WingedFace :: triangle (const WingedMesh& mesh) const {
  Triangle t (glm::vec3 (0.0f),glm::vec3 (0.0f),glm::vec3 (0.0f));
  this->triangle (mesh,t);
  return t;
}

glm::vec3 WingedFace :: normal (const WingedMesh& mesh) const {
  glm::vec3 v1 = this->_edge->vertex (*this,0)->vertex (mesh);
  glm::vec3 v2 = this->_edge->vertex (*this,1)->vertex (mesh);
  glm::vec3 v3 = this->_edge->vertex (*this,2)->vertex (mesh);

  return glm::normalize (glm::cross (v2-v1, v3-v2));
}

unsigned int WingedFace :: numEdges () const {
  unsigned int i = 0;

  for (ADJACENT_EDGE_ITERATOR(_,*this)) {
    i++;
  }
  return i;
}

LinkedEdge WingedFace :: adjacent (const WingedVertex& vertex) const {
  for (ADJACENT_EDGE_ITERATOR (it,*this)) {
    if (it.edge ()->isAdjacent (vertex))
      return it.edge ();
  }
  assert (false);
}

LinkedEdge WingedFace :: longestEdge (const WingedMesh& mesh) const {
  LinkedEdge longest   = this->edge ();
  float      maxLength = 0.0f;

  for (ADJACENT_EDGE_ITERATOR (it,*this)) {
    float length = it.edge ()->length (mesh);
    if (length > maxLength) {
      maxLength = length;
      longest   = it.edge ();
    }
  }
  return longest;
}

Maybe<LinkedVertex> WingedFace :: tVertex () const {
  for (ADJACENT_VERTEX_ITERATOR (it,*this)) {
    if (it.vertex ()->tEdge ().isDefined ())
      return Maybe<LinkedVertex> (it.vertex ());
  }
  return Maybe<LinkedVertex> ();
}

Maybe<LinkedEdge> WingedFace :: tEdge () const {
  for (ADJACENT_EDGE_ITERATOR (it,*this)) {
    if (it.edge ()->isTEdge ())
      return Maybe<LinkedEdge> (it.edge ());
  }
  return Maybe<LinkedEdge> ();
}

unsigned int WingedFace :: level () const {
  std::function <unsigned int (unsigned int)> levelMin;
  levelMin = [&levelMin,this] (unsigned int min) {
    unsigned int l = std::numeric_limits <unsigned int>::max ();
    for (ADJACENT_EDGE_ITERATOR (it,*this)) {
      if (it.edge ()->isTEdge ())
        return it.edge ()->vertex1 ()->level () - 1;
      if (it.edge ()->vertex1 ()->level () >= min)
        l = std::min <unsigned int> (l, it.edge ()->vertex1 ()->level ());
      if (it.edge ()->vertex2 ()->level () >= min)
        l = std::min <unsigned int> (l, it.edge ()->vertex2 ()->level ());
    }
    unsigned int count = 0;
    for (ADJACENT_VERTEX_ITERATOR (it,*this)) {
      if (it.vertex ()->level () == l)
        count++;
    }
    if (count == 1)
      return levelMin (l+1);
    else
      return l;
  };
  return levelMin (0);
}

LinkedVertex WingedFace :: highestLevelVertex () const {
  Maybe <LinkedVertex> v;
  for (ADJACENT_VERTEX_ITERATOR (it,*this)) {
    if (v.isUndefined () || it.vertex ()->level () > v.data ()->level ()) {
      v.data (it.vertex ());
    }
  }
  return v.data ();
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
AdjacentEdgeIterator WingedFace :: adjacentEdgeIterator ( LinkedEdge e
                                                        , bool skipT) const {
  return AdjacentEdgeIterator (*this,e,skipT);
}
AdjacentVertexIterator WingedFace :: adjacentVertexIterator ( LinkedEdge e
                                                            , bool skipT) const {
  return AdjacentVertexIterator (*this,e,skipT);
}
AdjacentFaceIterator WingedFace :: adjacentFaceIterator ( LinkedEdge e
                                                        , bool skipT) const {
  return AdjacentFaceIterator (*this,e,skipT);
}

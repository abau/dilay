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
#include "octree.hpp"

WingedFace :: WingedFace (IdType id) : _id (id), _octreeNode (nullptr) {}

WingedFace :: WingedFace (LinkedEdge e, IdType id) 
  : _id (id), _edge (e), _octreeNode (nullptr) {}

void WingedFace :: setEdge (LinkedEdge e) {
  this->_edge = e;
}

void WingedFace :: octreeNode (OctreeNode* node) {
  this->_octreeNode = node;
}

void WingedFace :: addIndices (WingedMesh& mesh) {
  for (ADJACENT_VERTEX_ITERATOR (it,*this)) {
    it.element ()->addIndex (mesh);
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

LinkedEdge WingedFace :: adjacent (const WingedVertex& vertex) const {
  for (ADJACENT_EDGE_ITERATOR (it,*this)) {
    if (it.element ()->isAdjacent (vertex))
      return it.element ();
  }
  assert (false);
}

LinkedEdge WingedFace :: longestEdge (const WingedMesh& mesh) const {
  LinkedEdge longest   = this->edge ();
  float      maxLength = 0.0f;

  for (ADJACENT_EDGE_ITERATOR (it,*this)) {
    float length = it.element ()->length (mesh);
    if (length > maxLength) {
      maxLength = length;
      longest   = it.element ();
    }
  }
  return longest;
}

Maybe<LinkedVertex> WingedFace :: tVertex () const {
  for (ADJACENT_VERTEX_ITERATOR (it,*this)) {
    if (it.element ()->tEdge ().isDefined ())
      return Maybe<LinkedVertex> (it.element ());
  }
  return Maybe<LinkedVertex> ();
}

Maybe<LinkedEdge> WingedFace :: tEdge () const {
  for (ADJACENT_EDGE_ITERATOR (it,*this)) {
    if (it.element ()->isTEdge ())
      return Maybe<LinkedEdge> (it.element ());
  }
  return Maybe<LinkedEdge> ();
}

unsigned int WingedFace :: level () const {
  std::function <unsigned int (unsigned int)> levelMin;
  levelMin = [&levelMin,this] (unsigned int min) {
    unsigned int l = std::numeric_limits <unsigned int>::max ();
    for (ADJACENT_EDGE_ITERATOR (it,*this)) {
      if (it.element ()->isTEdge ())
        return it.element ()->vertex1 ()->level () - 1;
      if (it.element ()->vertex1 ()->level () >= min)
        l = std::min <unsigned int> (l, it.element ()->vertex1 ()->level ());
      if (it.element ()->vertex2 ()->level () >= min)
        l = std::min <unsigned int> (l, it.element ()->vertex2 ()->level ());
    }
    unsigned int count = 0;
    for (ADJACENT_VERTEX_ITERATOR (it,*this)) {
      if (it.element ()->level () == l)
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
    if (v.isUndefined () || it.element ()->level () > v.data ()->level ()) {
      v.data (it.element ());
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

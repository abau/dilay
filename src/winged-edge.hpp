#ifndef DILAY_WINGED_EDGE
#define DILAY_WINGED_EDGE

#include "id.hpp"
#include "fwd-glm.hpp"
#include "macro.hpp"

class Id;
class WingedVertex;
class WingedFace;
class WingedMesh;

class WingedEdge {
  public:
    WingedEdge ();
    WingedEdge ( WingedVertex*, WingedVertex*, WingedFace*, WingedFace*
               , WingedEdge*, WingedEdge*, WingedEdge*, WingedEdge*
               , WingedEdge*, WingedEdge*);

    IdType        id               () const { return this->_id.get (); }

    WingedVertex* vertex1          () const { return this->_vertex1; }
    WingedVertex* vertex2          () const { return this->_vertex2; }
    WingedFace*   leftFace         () const { return this->_leftFace; }
    WingedFace*   rightFace        () const { return this->_rightFace; }
    WingedEdge*   leftPredecessor  () const { return this->_leftPredecessor; }
    WingedEdge*   leftSuccessor    () const { return this->_leftSuccessor; }
    WingedEdge*   rightPredecessor () const { return this->_rightPredecessor; }
    WingedEdge*   rightSuccessor   () const { return this->_rightSuccessor; }
    WingedEdge*   previousSibling  () const { return this->_previousSibling; }
    WingedEdge*   nextSibling      () const { return this->_nextSibling; }
    bool          isTEdge          () const { return this->_isTEdge; }

    bool          isLeftFace       (const WingedFace&)   const;
    bool          isRightFace      (const WingedFace&)   const;
    bool          isVertex1        (const WingedVertex&) const;
    bool          isVertex2        (const WingedVertex&) const;

    WingedVertex* firstVertex      (const WingedFace&)   const;
    WingedVertex* secondVertex     (const WingedFace&)   const;
    WingedEdge*   predecessor      (const WingedFace&)   const;
    WingedEdge*   successor        (const WingedFace&)   const;
    WingedFace*   otherFace        (const WingedFace&)   const;
    WingedVertex* otherVertex      (const WingedVertex&) const;

    void          setGeometry      ( WingedVertex*, WingedVertex*
                                   , WingedFace*, WingedFace*
                                   , WingedEdge*, WingedEdge*
                                   , WingedEdge*, WingedEdge*
                                   , WingedEdge* = nullptr
                                   , WingedEdge* = nullptr);

    void          vertex1          (WingedVertex* v) { this->_vertex1          = v; }
    void          vertex2          (WingedVertex* v) { this->_vertex2          = v; }
    void          leftFace         (WingedFace* f)   { this->_leftFace         = f; }
    void          rightFace        (WingedFace* f)   { this->_rightFace        = f; }
    void          leftPredecessor  (WingedEdge* e)   { this->_leftPredecessor  = e; }
    void          leftSuccessor    (WingedEdge* e)   { this->_leftSuccessor    = e; }
    void          rightPredecessor (WingedEdge* e)   { this->_rightPredecessor = e; }
    void          rightSuccessor   (WingedEdge* e)   { this->_rightSuccessor   = e; }
    void          previousSibling  (WingedEdge* e)   { this->_previousSibling  = e; }
    void          nextSibling      (WingedEdge* e)   { this->_nextSibling      = e; }
    void          isTEdge          (bool b)          { this->_isTEdge          = b; }

    void          firstVertex      (const WingedFace&, WingedVertex*);
    void          secondVertex     (const WingedFace&, WingedVertex*);
    void          face             (const WingedFace&, WingedFace*);
    void          predecessor      (const WingedFace&, WingedEdge*);
    void          successor        (const WingedFace&, WingedEdge*);

    glm::vec3     vector           (const WingedMesh&) const;
    float         length           (const WingedMesh&) const;
    float         lengthSqr        (const WingedMesh&) const;
    WingedEdge*   successor        (const WingedFace&, unsigned int) const;
    WingedEdge*   predecessor      (const WingedFace&, unsigned int) const;
    WingedVertex* vertex           (const WingedFace&, unsigned int) const;
    glm::vec3     middle           (const WingedMesh&) const;
    WingedEdge*   adjacent         (const WingedFace&, const WingedVertex&) const;
    bool          isAdjacent       (const WingedVertex&) const;
    bool          isFirstVertex    (const WingedFace&, const WingedVertex&) const;
    bool          isSecondVertex   (const WingedFace&, const WingedVertex&) const;
    WingedEdge*   adjacentSibling  (const WingedVertex&) const;

    SAFE_REF  (WingedVertex, vertex1)
    SAFE_REF  (WingedVertex, vertex2)
    SAFE_REF  (WingedFace  , leftFace)
    SAFE_REF  (WingedFace  , rightFace)
    SAFE_REF  (WingedEdge  , leftPredecessor)
    SAFE_REF  (WingedEdge  , leftSuccessor)
    SAFE_REF  (WingedEdge  , rightPredecessor)
    SAFE_REF  (WingedEdge  , rightSuccessor)
    SAFE_REF  (WingedEdge  , previousSibling)
    SAFE_REF  (WingedEdge  , nextSibling)

    SAFE_REF1 (WingedVertex, firstVertex, const WingedFace&)
    SAFE_REF1 (WingedVertex, secondVertex, const WingedFace&)
    SAFE_REF1 (WingedEdge  , predecessor, const WingedFace&)
    SAFE_REF1 (WingedEdge  , successor, const WingedFace&)
    SAFE_REF1 (WingedFace  , otherFace, const WingedFace&)
    SAFE_REF1 (WingedVertex, otherVertex, const WingedVertex&)

    SAFE_REF2 (WingedEdge  , successor, const WingedFace&, unsigned int)
    SAFE_REF2 (WingedEdge  , predecessor, const WingedFace&, unsigned int)
    SAFE_REF2 (WingedVertex, vertex, const WingedFace&, unsigned int)
    SAFE_REF2 (WingedEdge  , adjacent, const WingedFace&, const WingedVertex&)
    SAFE_REF1 (WingedEdge  , adjacentSibling, const WingedVertex&)

  private:
    const Id      _id;

    WingedVertex* _vertex1;
    WingedVertex* _vertex2;

    WingedFace*   _leftFace;
    WingedFace*   _rightFace;

    WingedEdge*   _leftPredecessor;
    WingedEdge*   _leftSuccessor;

    WingedEdge*   _rightPredecessor;
    WingedEdge*   _rightSuccessor;

    WingedEdge*   _previousSibling;
    WingedEdge*   _nextSibling;

    bool          _isTEdge;
};

#endif

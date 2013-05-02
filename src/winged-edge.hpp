#ifndef DILAY_WINGED_EDGE
#define DILAY_WINGED_EDGE

#include "id.hpp"
#include "fwd-winged.hpp"
#include "fwd-glm.hpp"
#include "maybe.hpp"

class Id;

class WingedEdge {
  public:
    WingedEdge ();
    WingedEdge ( LinkedVertex, LinkedVertex, LinkedFace, LinkedFace
               , LinkedEdge, LinkedEdge, LinkedEdge, LinkedEdge
               , const Maybe<LinkedEdge>&, const Maybe<LinkedEdge>&);

    IdType              id                () const { return this->_id.get (); }

    LinkedVertex        vertex1           () const { return this->_vertex1; }
    LinkedVertex        vertex2           () const { return this->_vertex2; }
    LinkedFace          leftFace          () const { return this->_leftFace; }
    LinkedFace          rightFace         () const { return this->_rightFace; }
    LinkedEdge          leftPredecessor   () const { return this->_leftPredecessor; }
    LinkedEdge          leftSuccessor     () const { return this->_leftSuccessor; }
    LinkedEdge          rightPredecessor  () const { return this->_rightPredecessor; }
    LinkedEdge          rightSuccessor    () const { return this->_rightSuccessor; }
    Maybe <LinkedEdge>  previousSibling   () const { return this->_previousSibling; }
    Maybe <LinkedEdge>  nextSibling       () const { return this->_nextSibling; }
    bool                isTEdge           () const { return this->_isTEdge; }

    bool                isLeftFace        (const WingedFace&)   const;
    bool                isRightFace       (const WingedFace&)   const;
    bool                isVertex1         (const WingedVertex&) const;
    bool                isVertex2         (const WingedVertex&) const;

    LinkedVertex        firstVertex       (const WingedFace&)   const;
    LinkedVertex        secondVertex      (const WingedFace&)   const;
    LinkedEdge          predecessor       (const WingedFace&)   const;
    LinkedEdge          successor         (const WingedFace&)   const;
    LinkedFace          otherFace         (const WingedFace&)   const;
    LinkedVertex        otherVertex       (const WingedVertex&) const;

    void          setGeometry         ( LinkedVertex, LinkedVertex
                                      , LinkedFace, LinkedFace
                                      , LinkedEdge, LinkedEdge
                                      , LinkedEdge, LinkedEdge
                                      , const Maybe <LinkedEdge>& = Maybe <LinkedEdge> ()
                                      , const Maybe <LinkedEdge>& = Maybe <LinkedEdge> ());
    void          setVertex1          (LinkedVertex);
    void          setVertex2          (LinkedVertex);
    void          setLeftFace         (LinkedFace);
    void          setRightFace        (LinkedFace);
    void          setLeftPredecessor  (LinkedEdge);
    void          setLeftSuccessor    (LinkedEdge);
    void          setRightPredecessor (LinkedEdge);
    void          setRightSuccessor   (LinkedEdge);
    void          setPreviousSibling  (const Maybe<LinkedEdge>&);
    void          setNextSibling      (const Maybe<LinkedEdge>&);
    void          isTEdge             (bool b) { this->_isTEdge = b; }

    void          setFirstVertex      (const WingedFace&, LinkedVertex);
    void          setSecondVertex     (const WingedFace&, LinkedVertex);
    void          setFace             (const WingedFace&, LinkedFace);
    void          setPredecessor      (const WingedFace&, LinkedEdge);
    void          setSuccessor        (const WingedFace&, LinkedEdge);

    glm::vec3     vector              (const WingedMesh&) const;
    float         length              (const WingedMesh&) const;
    float         lengthSqr           (const WingedMesh&) const;
    LinkedEdge    successor           (const WingedFace&, unsigned int) const;
    LinkedEdge    predecessor         (const WingedFace&, unsigned int) const;
    LinkedVertex  vertex              (const WingedFace&, unsigned int) const;
    glm::vec3     middle              (const WingedMesh&) const;
    LinkedEdge    adjacent            (const WingedFace&, const WingedVertex&) const;
    float         cosAngle            (const WingedMesh&, const WingedFace&
                                      ,const WingedVertex&) const;
    bool          isAdjacent          (const WingedVertex&) const;
    bool          isLeftOf            (const WingedFace&, const WingedVertex&) const;
    bool          isRightOf           (const WingedFace&, const WingedVertex&) const;
    Maybe <LinkedEdge> adjacentSibling (const WingedVertex&) const;

  private:
    const Id           _id;

    LinkedVertex       _vertex1;
    LinkedVertex       _vertex2;

    LinkedFace         _leftFace;
    LinkedFace         _rightFace;

    LinkedEdge         _leftPredecessor;
    LinkedEdge         _leftSuccessor;

    LinkedEdge         _rightPredecessor;
    LinkedEdge         _rightSuccessor;

    Maybe <LinkedEdge> _previousSibling;
    Maybe <LinkedEdge> _nextSibling;

    bool               _isTEdge;
};

#endif

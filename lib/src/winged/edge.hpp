#ifndef DILAY_WINGED_EDGE
#define DILAY_WINGED_EDGE

#include <glm/fwd.hpp>
#include "fwd-winged.hpp"
#include "macro.hpp"
#include "id.hpp"

class WingedVertex;
class WingedFace;
class WingedMesh;

enum class FaceGradient : char { None, Left, Right };

class WingedEdge {
  public:
    WingedEdge ();
    WingedEdge ( WingedVertex*, WingedVertex*, WingedFace*, WingedFace*  
               , WingedEdge*, WingedEdge*, WingedEdge*, WingedEdge*  
               , WingedEdge*, WingedEdge*, const Id&, bool
               , FaceGradient);
    WingedEdge (const WingedEdge&)  = default;
    WingedEdge (      WingedEdge&&) = default;

    const Id&       id               () const { return this->_id.id (); }
    WingedVertex*   vertex1          () const { return this->_vertex1; }
    WingedVertex*   vertex2          () const { return this->_vertex2; }
    WingedFace*     leftFace         () const { return this->_leftFace; }
    WingedFace*     rightFace        () const { return this->_rightFace; }
    WingedEdge*     leftPredecessor  () const { return this->_leftPredecessor; }
    WingedEdge*     leftSuccessor    () const { return this->_leftSuccessor; }
    WingedEdge*     rightPredecessor () const { return this->_rightPredecessor; }
    WingedEdge*     rightSuccessor   () const { return this->_rightSuccessor; }
    WingedEdge*     previousSibling  () const { return this->_previousSibling; }
    WingedEdge*     nextSibling      () const { return this->_nextSibling; }
    bool            isTEdge          () const { return this->_isTEdge; }
    FaceGradient    faceGradient     () const { return this->_faceGradient; }

    bool            isLeftFace       (const WingedFace&)   const;
    bool            isRightFace      (const WingedFace&)   const;
    bool            isVertex1        (const WingedVertex&) const;
    bool            isVertex2        (const WingedVertex&) const;

    WingedVertex*   firstVertex      (const WingedFace&)   const;
    WingedVertex*   secondVertex     (const WingedFace&)   const;
    WingedEdge*     predecessor      (const WingedFace&)   const;
    WingedEdge*     successor        (const WingedFace&)   const;
    WingedFace*     otherFace        (const WingedFace&)   const;
    WingedVertex*   otherVertex      (const WingedVertex&) const;

    void            setGeometry      ( WingedVertex*, WingedVertex*
                                     , WingedFace*, WingedFace*
                                     , WingedEdge*, WingedEdge*
                                     , WingedEdge*, WingedEdge*
                                     , WingedEdge* = nullptr
                                     , WingedEdge* = nullptr);

    void            vertex1          (WingedVertex* v) { this->_vertex1          = v; }
    void            vertex2          (WingedVertex* v) { this->_vertex2          = v; }
    void            leftFace         (WingedFace* f)   { this->_leftFace         = f; }
    void            rightFace        (WingedFace* f)   { this->_rightFace        = f; }
    void            leftPredecessor  (WingedEdge* e)   { this->_leftPredecessor  = e; }
    void            leftSuccessor    (WingedEdge* e)   { this->_leftSuccessor    = e; }
    void            rightPredecessor (WingedEdge* e)   { this->_rightPredecessor = e; }
    void            rightSuccessor   (WingedEdge* e)   { this->_rightSuccessor   = e; }
    void            previousSibling  (WingedEdge* e)   { this->_previousSibling  = e; }
    void            nextSibling      (WingedEdge* e)   { this->_nextSibling      = e; }
    void            isTEdge          (bool b)          { this->_isTEdge          = b; }
    void            faceGradient     (FaceGradient g)  { this->_faceGradient     = g; }

    void            firstVertex      (const WingedFace&, WingedVertex*);
    void            secondVertex     (const WingedFace&, WingedVertex*);
    void            face             (const WingedFace&, WingedFace*);
    void            predecessor      (const WingedFace&, WingedEdge*);
    void            successor        (const WingedFace&, WingedEdge*);

    glm::vec3       vector           (const WingedMesh&) const;
    float           length           (const WingedMesh&) const;
    float           lengthSqr        (const WingedMesh&) const;
    WingedEdge*     successor        (const WingedFace&, unsigned int) const;
    WingedEdge*     predecessor      (const WingedFace&, unsigned int) const;
    WingedVertex*   vertex           (const WingedFace&, unsigned int) const;
    glm::vec3       middle           (const WingedMesh&) const;
    WingedEdge*     adjacent         (const WingedFace&, const WingedVertex&) const;
    bool            isAdjacent       (const WingedVertex&) const;
    bool            isFirstVertex    (const WingedFace&, const WingedVertex&) const;
    bool            isSecondVertex   (const WingedFace&, const WingedVertex&) const;
    WingedEdge*     adjacentSibling  (const WingedVertex&) const;
    bool            gradientAlong    (const WingedFace&) const;

    SAFE_REF_CONST  (WingedVertex, vertex1)
    SAFE_REF_CONST  (WingedVertex, vertex2)
    SAFE_REF_CONST  (WingedFace  , leftFace)
    SAFE_REF_CONST  (WingedFace  , rightFace)
    SAFE_REF_CONST  (WingedEdge  , leftPredecessor)
    SAFE_REF_CONST  (WingedEdge  , leftSuccessor)
    SAFE_REF_CONST  (WingedEdge  , rightPredecessor)
    SAFE_REF_CONST  (WingedEdge  , rightSuccessor)
    SAFE_REF_CONST  (WingedEdge  , previousSibling)
    SAFE_REF_CONST  (WingedEdge  , nextSibling)

    SAFE_REF1_CONST (WingedVertex, firstVertex, const WingedFace&)
    SAFE_REF1_CONST (WingedVertex, secondVertex, const WingedFace&)
    SAFE_REF1_CONST (WingedEdge  , predecessor, const WingedFace&)
    SAFE_REF1_CONST (WingedEdge  , successor, const WingedFace&)
    SAFE_REF1_CONST (WingedFace  , otherFace, const WingedFace&)
    SAFE_REF1_CONST (WingedVertex, otherVertex, const WingedVertex&)

    SAFE_REF2_CONST (WingedEdge  , successor, const WingedFace&, unsigned int)
    SAFE_REF2_CONST (WingedEdge  , predecessor, const WingedFace&, unsigned int)
    SAFE_REF2_CONST (WingedVertex, vertex, const WingedFace&, unsigned int)
    SAFE_REF2_CONST (WingedEdge  , adjacent, const WingedFace&, const WingedVertex&)
    SAFE_REF1_CONST (WingedEdge  , adjacentSibling, const WingedVertex&)

  private:
    const IdObject  _id;

    WingedVertex*   _vertex1;
    WingedVertex*   _vertex2;

    WingedFace*     _leftFace;
    WingedFace*     _rightFace;

    WingedEdge*     _leftPredecessor;
    WingedEdge*     _leftSuccessor;

    WingedEdge*     _rightPredecessor;
    WingedEdge*     _rightSuccessor;

    WingedEdge*     _previousSibling;
    WingedEdge*     _nextSibling;

    bool            _isTEdge;
    FaceGradient    _faceGradient;
};

#endif

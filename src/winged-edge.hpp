#include "id.hpp"
#include "winged-vertex.hpp"
#include "winged-face.hpp"

#ifndef WINGED_EDGE
#define WINGED_EDGE

class WingedMesh;

class WingedEdge {
  public:
    WingedEdge ( LinkedVertex*, LinkedVertex*, LinkedFace*, LinkedFace*
               , LinkedEdge*, LinkedEdge*, LinkedEdge*, LinkedEdge*);

    IdType        id               () const { return this->_id.get (); }

    LinkedVertex* vertex1          () const { return this->_vertex1; }
    LinkedVertex* vertex2          () const { return this->_vertex2; }
    LinkedFace*   leftFace         () const { return this->_leftFace; }
    LinkedFace*   rightFace        () const { return this->_rightFace; }
    LinkedEdge*   leftPredecessor  () const { return this->_leftPredecessor; }
    LinkedEdge*   leftSuccessor    () const { return this->_leftSuccessor; }
    LinkedEdge*   rightPredecessor () const { return this->_rightPredecessor; }
    LinkedEdge*   rightSuccessor   () const { return this->_rightSuccessor; }

    bool          isLeftFace       (const WingedFace&)   const;
    bool          isRightFace      (const WingedFace&)   const;
    bool          isVertex1        (const WingedVertex&) const;
    bool          isVertex2        (const WingedVertex&) const;

    LinkedVertex* firstVertex         (const WingedFace&) const;
    LinkedVertex* secondVertex        (const WingedFace&) const;
    LinkedEdge*   predecessor         (const WingedFace&) const;
    LinkedEdge*   successor           (const WingedFace&) const;
    LinkedFace*   otherFace           (const WingedFace&) const;

    void          setVertex1          (LinkedVertex*);
    void          setVertex2          (LinkedVertex*);
    void          setLeftFace         (LinkedFace*);
    void          setRightFace        (LinkedFace*);
                                         
    void          setLeftPredecessor  (LinkedEdge*);
    void          setLeftSuccessor    (LinkedEdge*);
    void          setRightPredecessor (LinkedEdge*);
    void          setRightSuccessor   (LinkedEdge*);

    void          setPredecessor      (const WingedFace&, LinkedEdge*);
    void          setSuccessor        (const WingedFace&, LinkedEdge*);
    void          setFace             (const WingedFace&, LinkedFace*);

    float         lengthSqr           (const WingedMesh&) const;
    LinkedEdge*   successor           (const WingedFace&, unsigned int) const;
    LinkedVertex* vertex              (const WingedFace&, unsigned int) const;
  private:
    const Id      _id;

    LinkedVertex* _vertex1;
    LinkedVertex* _vertex2;

    LinkedFace*   _leftFace;
    LinkedFace*   _rightFace;

    LinkedEdge*   _leftPredecessor;
    LinkedEdge*   _leftSuccessor;

    LinkedEdge*   _rightPredecessor;
    LinkedEdge*   _rightSuccessor;
};

#endif

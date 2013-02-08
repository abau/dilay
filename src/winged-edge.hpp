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

    IdType              id               () const { return this->_id.get (); }

    LinkedVertex*       vertex1          ()       { return this->_vertex1; }
    LinkedVertex*       vertex2          ()       { return this->_vertex2; }
    LinkedFace*         leftFace         ()       { return this->_leftFace; }
    LinkedFace*         rightFace        ()       { return this->_rightFace; }
    LinkedEdge*         leftPredecessor  ()       { return this->_leftPredecessor; }
    LinkedEdge*         leftSuccessor    ()       { return this->_leftSuccessor; }
    LinkedEdge*         rightPredecessor ()       { return this->_rightPredecessor; }
    LinkedEdge*         rightSuccessor   ()       { return this->_rightSuccessor; }

    CONST0(LinkedVertex*, vertex1)
    CONST0(LinkedVertex*, vertex2)
    CONST0(LinkedFace*  , leftFace)
    CONST0(LinkedFace*  , rightFace)
    CONST0(LinkedEdge*  , leftPredecessor)
    CONST0(LinkedEdge*  , leftSuccessor)
    CONST0(LinkedEdge*  , rightPredecessor)
    CONST0(LinkedEdge*  , rightSuccessor)

    bool                isLeftFace          (const WingedFace&) const;
    bool                isRightFace         (const WingedFace&) const;

    LinkedVertex*       firstVertex         (const WingedFace&);
    LinkedVertex*       secondVertex        (const WingedFace&);
    LinkedEdge*         predecessor         (const WingedFace&);
    LinkedEdge*         successor           (const WingedFace&);
    LinkedFace*         otherFace           (const WingedFace&);

    CONST1(LinkedVertex*, firstVertex , const WingedFace&, f) 
    CONST1(LinkedVertex*, secondVertex, const WingedFace&, f)
    CONST1(LinkedEdge*  , predecessor , const WingedFace&, f)
    CONST1(LinkedEdge*  , successor   , const WingedFace&, f)
    CONST1(LinkedFace*  , otherFace   , const WingedFace&, f)

    void                setVertex1          (LinkedVertex*);
    void                setVertex2          (LinkedVertex*);
    void                setLeftFace         (LinkedFace*);
    void                setRightFace        (LinkedFace*);
                                               
    void                setLeftPredecessor  (LinkedEdge*);
    void                setLeftSuccessor    (LinkedEdge*);
    void                setRightPredecessor (LinkedEdge*);
    void                setRightSuccessor   (LinkedEdge*);

    void                setPredecessor      (const WingedFace&, LinkedEdge*);
    void                setSuccessor        (const WingedFace&, LinkedEdge*);
    void                setFace             (const WingedFace&, LinkedFace*);

    float               lengthSqr           (const WingedMesh&) const;
    LinkedEdge*         successor           (const WingedFace&, unsigned int);

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

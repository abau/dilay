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

    const LinkedVertex* vertex1          () const { return this->_vertex1; }
    const LinkedVertex* vertex2          () const { return this->_vertex2; }
    const LinkedFace*   leftFace         () const { return this->_leftFace; }
    const LinkedFace*   rightFace        () const { return this->_rightFace; }
    const LinkedEdge*   leftPredecessor  () const { return this->_leftPredecessor; }
    const LinkedEdge*   leftSuccessor    () const { return this->_leftSuccessor; }
    const LinkedEdge*   rightPredecessor () const { return this->_rightPredecessor; }
    const LinkedEdge*   rightSuccessor   () const { return this->_rightSuccessor; }

    bool                isLeftFace          (const WingedFace&) const;
    bool                isRightFace         (const WingedFace&) const;

    LinkedVertex*       firstVertex         (const WingedFace&);
    LinkedVertex*       secondVertex        (const WingedFace&);
    LinkedEdge*         predecessor         (const WingedFace&);
    LinkedEdge*         successor           (const WingedFace&);

    const LinkedVertex* firstVertex         (const WingedFace&) const;
    const LinkedVertex* secondVertex        (const WingedFace&) const;
    const LinkedEdge*   predecessor         (const WingedFace&) const;
    const LinkedEdge*   successor           (const WingedFace&) const;

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

#ifndef DILAY_ACTION_IDENTIFIER
#define DILAY_ACTION_IDENTIFIER

#include "macro.hpp"

class WingedMesh;
class WingedFace;
class WingedEdge;
class WingedVertex;

class ActionIdentifier {
  public: 
    DECLARE_BIG6     (ActionIdentifier)

    ActionIdentifier (unsigned int);
    ActionIdentifier (const WingedMesh*);
    ActionIdentifier (const WingedFace*);
    ActionIdentifier (const WingedEdge*);
    ActionIdentifier (const WingedVertex*);
    ActionIdentifier (const WingedMesh&);
    ActionIdentifier (const WingedFace&);
    ActionIdentifier (const WingedEdge&);
    ActionIdentifier (const WingedVertex&);

    void            setIndex          (unsigned int);
    void            setMesh           (const WingedMesh*);
    void            setFace           (const WingedFace*);
    void            setEdge           (const WingedEdge*);
    void            setVertex         (const WingedVertex*);

    bool            isSet             () const;
    unsigned int*   getIndex          () const;
    WingedMesh*     getWingedMesh     () const;
    WingedFace*     getFace           (const WingedMesh&) const;
    WingedEdge*     getEdge           (const WingedMesh&) const;
    WingedVertex*   getVertex         (const WingedMesh&) const;

    template <typename T>
    T*              getMesh           () const;

    SAFE_REF_CONST  (unsigned int  , getIndex)
    SAFE_REF_CONST  (WingedMesh    , getWingedMesh)
    SAFE_REF1_CONST (WingedFace    , getFace  , const WingedMesh&)
    SAFE_REF1_CONST (WingedEdge    , getEdge  , const WingedMesh&)
    SAFE_REF1_CONST (WingedVertex  , getVertex, const WingedMesh&)

  private:
    IMPLEMENTATION
};
#endif

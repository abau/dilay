#ifndef DILAY_ACTION_IDENTIFIER
#define DILAY_ACTION_IDENTIFIER

#include "macro.hpp"

class WingedMesh;
class WingedFace;
class WingedEdge;
class WingedVertex;
class Id;
class SphereMesh;
class SphereMeshNode;

class ActionIdentifier {
  public: 
    DECLARE_BIG6 (ActionIdentifier)

    ActionIdentifier (const Id&);
    ActionIdentifier (unsigned int);
    ActionIdentifier (const WingedMesh*);
    ActionIdentifier (const SphereMesh*);
    ActionIdentifier (const SphereMeshNode*);
    ActionIdentifier (const WingedFace*);
    ActionIdentifier (const WingedEdge*);
    ActionIdentifier (const WingedVertex*);
    ActionIdentifier (const WingedMesh&);
    ActionIdentifier (const SphereMesh&);
    ActionIdentifier (const SphereMeshNode&);
    ActionIdentifier (const WingedFace&);
    ActionIdentifier (const WingedEdge&);
    ActionIdentifier (const WingedVertex&);

    void            setId             (const Id&);
    void            setIndex          (unsigned int);
    void            setMesh           (const WingedMesh*);
    void            setMesh           (const SphereMesh*);
    void            setNode           (const SphereMeshNode*);
    void            setFace           (const WingedFace*);
    void            setEdge           (const WingedEdge*);
    void            setVertex         (const WingedVertex*);

    bool            isSet             () const;
    Id*             getId             () const;
    unsigned int*   getIndex          () const;
    WingedMesh*     getWingedMesh     () const;
    SphereMesh*     getSphereMesh     () const;
    SphereMeshNode* getSphereMeshNode (const SphereMesh&) const;
    WingedFace*     getFace           (const WingedMesh&) const;
    WingedEdge*     getEdge           (const WingedMesh&) const;
    WingedVertex*   getVertex         (const WingedMesh&) const;

    template <typename T>
    T*              getMesh           () const;

    SAFE_REF_CONST  (Id          , getId)
    SAFE_REF_CONST  (unsigned int, getIndex)
    SAFE_REF_CONST  (WingedMesh  , getWingedMesh)
    SAFE_REF_CONST  (SphereMesh  , getSphereMesh)
    SAFE_REF1_CONST (WingedFace  , getFace  , WingedMesh&)
    SAFE_REF1_CONST (WingedEdge  , getEdge  , WingedMesh&)
    SAFE_REF1_CONST (WingedVertex, getVertex, WingedMesh&)

  private:
    IMPLEMENTATION
};
#endif

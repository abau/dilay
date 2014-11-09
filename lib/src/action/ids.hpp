#ifndef DILAY_ACTION_IDS
#define DILAY_ACTION_IDS

#include <initializer_list>
#include "macro.hpp"

class WingedMesh;
class WingedFace;
class WingedEdge;
class WingedVertex;
class Id;
class SphereMesh;
class SphereMeshNode;

class ActionIds {
  public: 
    DECLARE_BIG3 (ActionIds)

    unsigned int    numIds            () const;
    unsigned int    numIndices        () const;
    void            setId             (unsigned int, const Id&);
    void            setIds            (std::initializer_list <Id>, unsigned int = 0);
    void            setIndex          (unsigned int, unsigned int);
    void            setIndices        (std::initializer_list <unsigned int>, unsigned int = 0);
    Id*             getId             (unsigned int) const;
    unsigned int*   getIndex          (unsigned int) const;
    WingedMesh&     getWingedMesh     (unsigned int) const;
    SphereMesh&     getSphereMesh     (unsigned int) const;
    SphereMeshNode* getSphereMeshNode (SphereMesh&, unsigned int) const;
    WingedFace*     getFace           (WingedMesh&, unsigned int) const;
    WingedEdge*     getEdge           (WingedMesh&, unsigned int) const;
    WingedVertex*   getVertex         (WingedMesh&, unsigned int) const;
    void            setMesh           (unsigned int, const WingedMesh&);
    void            setMesh           (unsigned int, const SphereMesh&);
    void            setNode           (unsigned int, const SphereMeshNode*);
    void            setFace           (unsigned int, const WingedFace*);
    void            setEdge           (unsigned int, const WingedEdge*);
    void            setVertex         (unsigned int, const WingedVertex*);

    template <typename T>
    T&              getMesh           (unsigned int) const;

    SAFE_REF1_CONST (Id          , getId    , unsigned int)
    SAFE_REF1_CONST (unsigned int, getIndex , unsigned int)
    SAFE_REF2_CONST (WingedFace  , getFace  , WingedMesh&, unsigned int)
    SAFE_REF2_CONST (WingedEdge  , getEdge  , WingedMesh&, unsigned int)
    SAFE_REF2_CONST (WingedVertex, getVertex, WingedMesh&, unsigned int)

  private:
    IMPLEMENTATION
};
#endif

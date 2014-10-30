#ifndef DILAY_AFFECTED_FACES
#define DILAY_AFFECTED_FACES

#include "fwd-winged.hpp"
#include "macro.hpp"

class AffectedFaces {
  public:
    DECLARE_BIG6 (AffectedFaces)

    void         insert          (WingedFace&);
    void         insert          (const AffectedFaces&);
    void         remove          (WingedFace&);
    void         remove          (WingedEdge&);
    void         reset           ();
    void         commit          ();
    bool         contains        (WingedFace&) const;
    bool         contains        (WingedFace*) const;
    bool         contains        (WingedEdge&) const;
    bool         contains        (WingedEdge*) const;
    VertexPtrSet toVertexSet     () const;
    FacePtrSet&  faces           () const;
    FacePtrSet&  uncommitedFaces () const;
    EdgePtrSet&  edges           () const;

  private:
    IMPLEMENTATION
};

#endif

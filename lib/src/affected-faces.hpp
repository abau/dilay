/* This file is part of Dilay
 * Copyright © 2015,2016 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_AFFECTED_FACES
#define DILAY_AFFECTED_FACES

#include <glm/fwd.hpp>
#include "macro.hpp"
#include "winged/fwd.hpp"

class AffectedFaces {
  public:
    DECLARE_BIG6 (AffectedFaces)

    void insert         (WingedFace&);
    void insert         (const AffectedFaces&);
    void remove         (WingedFace&);
    void reset          ();
    void resetCommitted ();
    void commit         ();
    bool isEmpty        () const;
    bool contains       (WingedFace&) const;
    bool contains       (WingedFace*) const;

    const FacePtrSet&  faces            () const;
    const FacePtrSet&  uncommittedFaces () const;
          VertexPtrSet toVertexSet      () const;
          EdgePtrVec   toEdgeVec        () const;

  private:
    IMPLEMENTATION
};

#endif

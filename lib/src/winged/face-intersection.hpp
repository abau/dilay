/* This file is part of Dilay
 * Copyright © 2015,2016 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_WINGED_FACE_INTERSECTION
#define DILAY_WINGED_FACE_INTERSECTION

#include "intersection.hpp"
#include "macro.hpp"

class WingedFace;
class WingedMesh;

class WingedFaceIntersection : public Intersection {
  public:
    DECLARE_BIG6 (WingedFaceIntersection)

    WingedMesh& mesh   () const;
    WingedFace& face   () const;
    void        update ( float, const glm::vec3&, const glm::vec3&
                       , WingedMesh&, WingedFace& );

  private:
    IMPLEMENTATION
};

#endif

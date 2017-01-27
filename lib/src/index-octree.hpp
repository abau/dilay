/* This file is part of Dilay
 * Copyright © 2015,2016 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_INDEX_OCTREE
#define DILAY_INDEX_OCTREE

#include <glm/fwd.hpp>
#include <functional>
#include "macro.hpp"

class Camera;
class PrimAABox;
class PrimPlane;
class PrimRay;
class PrimSphere;

class IndexOctree { 
  public: 
    DECLARE_BIG4_EXPLICIT_COPY (IndexOctree)

    typedef std::function <void (unsigned int)> IntersectionCallback;

    bool             hasRoot                () const;
    void             setupRoot              (const glm::vec3&, float);
    void             addElement             (unsigned int, const glm::vec3&, float);
    void             realignElement         (unsigned int, const glm::vec3&, float);
    void             addDegeneratedElement  (unsigned int);
    void             deleteElement          (unsigned int);
    void             deleteEmptyChildren    ();
    void             shrinkRoot             ();
    void             reset                  ();
    void             render                 (Camera&);
    void             intersects             (const PrimRay&, const IntersectionCallback&) const;
    void             intersects             (const PrimSphere&, const IntersectionCallback&) const;
    void             intersects             (const PrimPlane&, const IntersectionCallback&) const;
    void             intersects             (const PrimAABox&, const IntersectionCallback&) const;
    unsigned int     numDegeneratedElements () const;
    unsigned int     someDegeneratedElement () const;
    void             printStatistics        () const;

  private:
    IMPLEMENTATION
};

#endif

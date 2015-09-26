/* This file is part of Dilay
 * Copyright © 2015 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_INDEX_OCTREE
#define DILAY_INDEX_OCTREE

#include <glm/fwd.hpp>
#include <vector>
#include "macro.hpp"

class Camera;
class PrimRay;
class PrimSphere;

class IndexOctree { 
  public: 
    DECLARE_BIG4COPY (IndexOctree)

    typedef std::function <void (unsigned int)> IntersectionCallback;

    bool             hasRoot                () const;
    void             setupRoot              (const glm::vec3&, float);
    void             addElement             (unsigned int, const glm::vec3&, float);
    void             addDegeneratedElement  (unsigned int);
    void             deleteElement          (unsigned int);
    void             deleteEmptyChildren    ();
    void             shrinkRoot             ();
    void             reset                  ();
    void             render                 (Camera&);
    void             intersects             (const PrimRay&, const IntersectionCallback&) const;
    void             intersects             (const PrimSphere&, const IntersectionCallback&) const;
    unsigned int     numDegeneratedElements () const;
    unsigned int     someDegeneratedElement () const;
    void             rewriteIndices         (const std::vector <unsigned int>&);
    void             printStatistics        () const;

  private:
    IMPLEMENTATION
};

#endif

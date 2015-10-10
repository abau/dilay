/* This file is part of Dilay
 * Copyright © 2015 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_SKETCH_PATH
#define DILAY_SKETCH_PATH

#include <glm/fwd.hpp>
#include "macro.hpp"
#include "sketch/fwd.hpp"

class Camera;
class Intersection;
class Mesh;
class PrimPlane;
class PrimRay;

class SketchPath {
  public:
    using Spheres = std::vector <PrimSphere>;

    DECLARE_BIG6 (SketchPath);

    const Spheres&   spheres    () const;
    const glm::vec3& minimum    () const;
    const glm::vec3& maximum    () const;
    bool             isEmpty    () const;
    void             addSphere  (const glm::vec3&, float);
    void             render     (Camera&, Mesh&) const;
    bool             intersects (const PrimRay&, Intersection&) const;
    SketchPath       mirror     (const PrimPlane&);
    void             smooth     (const glm::vec3&, float, unsigned int);

  private:
    IMPLEMENTATION
};

#endif

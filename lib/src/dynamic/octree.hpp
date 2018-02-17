/* This file is part of Dilay
 * Copyright © 2015-2018 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_DYNAMIC_OCTREE
#define DILAY_DYNAMIC_OCTREE

#include <functional>
#include <glm/fwd.hpp>
#include <vector>
#include "macro.hpp"

class Camera;
class PrimAABox;
class PrimPlane;
class PrimRay;
class PrimSphere;

class DynamicOctree
{
public:
  DECLARE_BIG4_EXPLICIT_COPY (DynamicOctree)

  typedef std::function<void(unsigned int)>       IntersectionCallback;
  typedef std::function<float(unsigned int)>      RayIntersectionCallback;
  typedef std::function<void(bool, unsigned int)> ContainsIntersectionCallback;
  typedef std::function<float(unsigned int)>      DistanceCallback;

  bool  hasRoot () const;
  void  setupRoot (const glm::vec3&, float);
  void  addElement (unsigned int, const glm::vec3&, float);
  void  realignElement (unsigned int, const glm::vec3&, float);
  void  deleteElement (unsigned int);
  void  deleteEmptyChildren ();
  void  updateIndices (const std::vector<unsigned int>&);
  void  shrinkRoot ();
  void  reset ();
  void  render (Camera&) const;
  void  intersects (const PrimRay&, const RayIntersectionCallback&) const;
  void  intersects (const PrimPlane&, const IntersectionCallback&) const;
  void  intersects (const PrimSphere&, const ContainsIntersectionCallback&) const;
  void  intersects (const PrimAABox&, const ContainsIntersectionCallback&) const;
  float distance (const glm::vec3&, const DistanceCallback&) const;
  void  printStatistics () const;

private:
  IMPLEMENTATION
};

#endif

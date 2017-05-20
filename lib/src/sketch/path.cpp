#include "../mesh.hpp"
#include "intersection.hpp"
#include "primitive/aabox.hpp"
#include "primitive/plane.hpp"
#include "primitive/ray.hpp"
#include "sketch/path-intersection.hpp"
#include "sketch/path.hpp"
#include "util.hpp"

struct SketchPath::Impl
{
  SketchPath*         self;
  SketchPath::Spheres spheres;
  glm::vec3           minimum;
  glm::vec3           maximum;
  glm::vec3           intersectionFirst;
  glm::vec3           intersectionLast;

  Impl (SketchPath* s)
    : self (s)
  {
    this->resetMinMax ();
  }

  void resetMinMax ()
  {
    this->minimum = glm::vec3 (Util::maxFloat ());
    this->maximum = glm::vec3 (Util::minFloat ());
  }

  void reset ()
  {
    this->resetMinMax ();
    this->spheres.clear ();
  }

  void setMinMax ()
  {
    this->resetMinMax ();

    for (const PrimSphere& s : this->spheres)
    {
      this->maximum = glm::max (this->maximum, s.center () + glm::vec3 (s.radius ()));
      this->minimum = glm::min (this->minimum, s.center () - glm::vec3 (s.radius ()));
    }
  }

  bool isEmpty () const { return this->spheres.empty (); }

  PrimAABox aabox () const
  {
    assert (this->isEmpty () == false);
    return PrimAABox (this->minimum, this->maximum);
  }

  void addSphere (const glm::vec3& intersection, const glm::vec3& position, float radius)
  {
    if (this->spheres.empty ())
    {
      this->intersectionFirst = intersection;
      this->intersectionLast = intersection;
    }
    else
    {
      this->intersectionLast = intersection;
    }
    this->maximum = glm::max (this->maximum, position + glm::vec3 (radius));
    this->minimum = glm::min (this->minimum, position - glm::vec3 (radius));

    this->spheres.emplace_back (position, radius);
  }

  SketchPath::Spheres::iterator deleteSphere (SketchPath::Spheres::const_iterator it)
  {
    return this->spheres.erase (it);
  }

  void render (Camera& camera, Mesh& mesh) const
  {
    for (const PrimSphere& s : this->spheres)
    {
      mesh.position (s.center ());
      mesh.scaling (glm::vec3 (s.radius ()));
      mesh.render (camera);
    }
  }

  bool intersects (const PrimRay& ray, SketchMesh& mesh, SketchPathIntersection& intersection)
  {
    if (IntersectionUtil::intersects (ray, PrimAABox (this->minimum, this->maximum)))
    {
      for (const PrimSphere& s : this->spheres)
      {
        float t;
        if (IntersectionUtil::intersects (ray, s, &t))
        {
          intersection.update (t, ray.pointAt (t), glm::normalize (ray.pointAt (t) - s.center ()),
                               mesh, *this->self);
        }
      }
    }
    return intersection.isIntersection ();
  }

  SketchPath mirror (const PrimPlane& mPlane)
  {
    SketchPath::Spheres oldSpheres (std::move (this->spheres));
    const glm::vec3     oldIntersectionFirst (this->intersectionFirst);
    const glm::vec3     oldIntersectionLast (this->intersectionLast);
    SketchPath          mirrored;

    this->reset ();
    for (unsigned int i = 0; i < oldSpheres.size (); i++)
    {
      const PrimSphere& s = oldSpheres[i];

      if (mPlane.distance (s.center ()) > -Util::epsilon ())
      {
        const glm::vec3 mCenter = mPlane.mirror (s.center ());

        if (i == 0)
        {
          this->addSphere (oldIntersectionFirst, s.center (), s.radius ());
          mirrored.addSphere (mPlane.mirror (oldIntersectionFirst), mCenter, s.radius ());
        }
        else if (i == oldSpheres.size () - 1)
        {
          this->addSphere (oldIntersectionLast, s.center (), s.radius ());
          mirrored.addSphere (mPlane.mirror (oldIntersectionLast), mCenter, s.radius ());
        }
        else
        {
          this->addSphere (s.center (), s.center (), s.radius ());
          mirrored.addSphere (mCenter, mCenter, s.radius ());
        }
      }
    }
    return mirrored;
  }

  void smooth (const PrimSphere& range, unsigned int halfWidth, SketchPathSmoothEffect effect,
               const PrimSphere* nearestToFirst, const PrimSphere* nearestToLast)
  {
    const unsigned int numS = this->spheres.size ();

    for (unsigned int i = 0; i < numS; i++)
    {
      if (IntersectionUtil::intersects (range, this->spheres.at (i)))
      {
        const unsigned int hW =
          i < halfWidth ? i : (i >= numS - halfWidth ? numS - i - 1 : halfWidth);
        glm::vec3 center (0.0f);
        float     radius (0.0f);
        for (unsigned int j = i - hW; j <= i + hW; j++)
        {
          center += this->spheres.at (j).center ();
          radius += this->spheres.at (j).radius ();
        }

        const bool effectEmbeds = effect == SketchPathSmoothEffect::Embed ||
                                  effect == SketchPathSmoothEffect::EmbedAndAdjust;
        unsigned int numAffectedCenter = 0;
        unsigned int numAffectedRadius = 0;

        if (effect != SketchPathSmoothEffect::None)
        {
          if (i < halfWidth)
          {
            if (nearestToFirst && effectEmbeds)
            {
              numAffectedCenter++;
              center += nearestToFirst->center ();
            }

            if (nearestToFirst && effect == SketchPathSmoothEffect::EmbedAndAdjust)
            {
              numAffectedRadius++;
              radius += nearestToFirst->radius ();
            }
            else if (effect == SketchPathSmoothEffect::Pinch)
            {
              numAffectedRadius++;
              numAffectedCenter++;
              center += this->intersectionFirst;
            }
          }

          if (i >= numS - halfWidth)
          {
            if (nearestToLast && effectEmbeds)
            {
              numAffectedCenter++;
              center += nearestToLast->center ();
            }

            if (nearestToLast && effect == SketchPathSmoothEffect::EmbedAndAdjust)
            {
              numAffectedRadius++;
              radius += nearestToLast->radius ();
            }
            else if (effect == SketchPathSmoothEffect::Pinch)
            {
              numAffectedRadius++;
              numAffectedCenter++;
              center += this->intersectionLast;
            }
          }
        }
        this->spheres.at (i).center (center / float((2 * hW) + 1 + numAffectedCenter));
        this->spheres.at (i).radius (radius / float((2 * hW) + 1 + numAffectedRadius));
      }
    }
    this->setMinMax ();
  }
};

DELEGATE_BIG6_SELF (SketchPath)
GETTER_CONST (const SketchPath::Spheres&, SketchPath, spheres)
GETTER_CONST (const glm::vec3&, SketchPath, minimum)
GETTER_CONST (const glm::vec3&, SketchPath, maximum)
GETTER_CONST (const glm::vec3&, SketchPath, intersectionFirst)
GETTER_CONST (const glm::vec3&, SketchPath, intersectionLast)
SETTER (const glm::vec3&, SketchPath, intersectionFirst)
SETTER (const glm::vec3&, SketchPath, intersectionLast)
DELEGATE (void, SketchPath, reset)
DELEGATE_CONST (bool, SketchPath, isEmpty)
DELEGATE_CONST (PrimAABox, SketchPath, aabox)
DELEGATE3 (void, SketchPath, addSphere, const glm::vec3&, const glm::vec3&, float)
DELEGATE1 (SketchPath::Spheres::iterator, SketchPath, deleteSphere,
           SketchPath::Spheres::const_iterator)
DELEGATE2_CONST (void, SketchPath, render, Camera&, Mesh&)
DELEGATE3 (bool, SketchPath, intersects, const PrimRay&, SketchMesh&, SketchPathIntersection&)
DELEGATE1 (SketchPath, SketchPath, mirror, const PrimPlane&)
DELEGATE5 (void, SketchPath, smooth, const PrimSphere&, unsigned int, SketchPathSmoothEffect,
           const PrimSphere*, const PrimSphere*)

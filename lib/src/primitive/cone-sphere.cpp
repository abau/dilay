/* This file is part of Dilay
 * Copyright © 2015-2017 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <glm/gtc/constants.hpp>
#include <sstream>
#include "primitive/cone-sphere.hpp"
#include "primitive/cone.hpp"
#include "util.hpp"

PrimConeSphere::PrimConeSphere (const PrimSphere& s1, const PrimSphere& s2)
  : _sphere1 (s1.radius () > s2.radius () ? s1 : s2)
  , _sphere2 (s1.radius () > s2.radius () ? s2 : s1)
  , _length (glm::distance (s1.center (), s2.center ()))
  , _direction ((this->_sphere2.center () - this->_sphere1.center ()) / glm::vec3 (this->_length))
  , _alpha (glm::half_pi<float> () - glm::acos (this->delta () / this->_length))
  , _sinAlpha (glm::sin (this->_alpha))
  , _cosAlpha (glm::cos (this->_alpha))
{
}

float PrimConeSphere::delta () const { return this->_sphere1.radius () - this->_sphere2.radius (); }

bool PrimConeSphere::sameRadii () const { return Util::almostEqual (this->delta (), 0.0f); }

bool PrimConeSphere::hasCone () const { return this->_length > this->delta (); }

float PrimConeSphere::coneSideLength () const
{
  assert (this->hasCone ());

  return glm::sqrt ((this->_length * this->_length) - (this->delta () * this->delta ()));
}

PrimCone PrimConeSphere::toCone () const
{
  assert (this->hasCone ());

  const float s = this->coneSideLength ();
  const float h1 = this->_sphere1.radius () * this->delta () / this->_length;
  const float h2 = this->_sphere2.radius () * this->delta () / this->_length;
  const float r1c = this->_sphere1.radius () * s / this->_length;
  const float r2c = this->_sphere2.radius () * s / this->_length;

  return PrimCone (this->_sphere2.center () + (this->_direction * h2), r2c,
                   this->_sphere1.center () + (this->_direction * h1), r1c);
}

std::ostream& operator<< (std::ostream& os, const PrimConeSphere& coneSphere)
{
  os << "PrimConeSphere { sphere1 = " << (coneSphere.sphere1 ())
     << ", sphere2 = " << (coneSphere.sphere2 ()) << " }";
  return os;
}

/* This file is part of Dilay
 * Copyright © 2015-2017 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_TOOL_SCULPT_BRUSH
#define DILAY_TOOL_SCULPT_BRUSH

#include <glm/glm.hpp>
#include "macro.hpp"
#include "maybe.hpp"

class DynamicFaces;
class DynamicMesh;
class PrimPlane;
class PrimSphere;
class SculptBrush;

class SBParameters
{
public:
  virtual ~SBParameters () {}

  virtual float intensity () const { return 0.0f; }

  virtual void intensity (float) {}

  virtual bool useLastPos () const { return false; }

  virtual bool discardBack () const { return true; }

  virtual bool reduce () const { return false; }

  virtual void mirror (const PrimPlane&) {}

  virtual void sculpt (const SculptBrush&, const DynamicFaces&) const = 0;
};

class SBIntensityParameter : virtual public SBParameters
{
public:
  SBIntensityParameter ()
    : _intensity (0.0f)
  {
  }

  MEMBER_GETTER_SETTER (float, intensity);
};

class SBInvertParameter : virtual public SBParameters
{
public:
  SBInvertParameter ()
    : _invert (false)
  {
  }

  void toggleInvert () { this->_invert = !this->_invert; }

  glm::vec3 invert (const glm::vec3& v) const { return this->_invert ? -v : v; }

  MEMBER_GETTER_SETTER (bool, invert);
};

class SBDiscardBackParameter : virtual public SBParameters
{
public:
  SBDiscardBackParameter ()
    : _discardBack (true)
  {
  }

  bool discardBack () const override { return this->_discardBack; }
  void discardBack (bool d) { this->_discardBack = d; }

private:
  bool _discardBack;
};

class SBDrawParameters : public SBIntensityParameter, public SBInvertParameter
{
public:
  SBDrawParameters ()
    : _flat (true)
  {
  }

  void sculpt (const SculptBrush&, const DynamicFaces&) const;

  MEMBER_GETTER_SETTER (bool, flat);
  MEMBER_GETTER_SETTER (bool, constantHeight);
};

class SBGrablikeParameters : public SBDiscardBackParameter
{
public:
  void sculpt (const SculptBrush&, const DynamicFaces&) const override;

  bool useLastPos () const override { return true; }
};

class SBSmoothParameters : public SBIntensityParameter
{
public:
  void sculpt (const SculptBrush&, const DynamicFaces&) const;
};

class SBReduceParameters : public SBIntensityParameter
{
public:
  bool reduce () const override { return true; }

  void sculpt (const SculptBrush&, const DynamicFaces&) const override;
};

class SBFlattenParameters : public SBIntensityParameter
{
public:
  SBFlattenParameters ();

  void sculpt (const SculptBrush&, const DynamicFaces&) const override;

  bool             hasLockedPlane () const;
  const PrimPlane& lockedPlane () const;
  void             lockedPlane (const PrimPlane& p);
  void             resetLockedPlane ();
  void             mirror (const PrimPlane&) override;

  MEMBER_GETTER_SETTER (bool, lockPlane);

private:
  Maybe<PrimPlane> _lockedPlane;
};

class SBCreaseParameters : public SBIntensityParameter, public SBInvertParameter
{
public:
  void sculpt (const SculptBrush&, const DynamicFaces&) const;
};

class SBPinchParameters : public SBInvertParameter
{
public:
  void sculpt (const SculptBrush&, const DynamicFaces&) const;
};

class SculptBrush
{
public:
  DECLARE_BIG3 (SculptBrush)

  float        radius () const;
  float        detailFactor () const;
  float        stepWidthFactor () const;
  bool         subdivide () const;
  bool         hasMesh () const;
  DynamicMesh& mesh () const;

  void radius (float);
  void detailFactor (float);
  void stepWidthFactor (float);
  void subdivide (bool);

  float            subdivThreshold () const;
  const glm::vec3& lastPosition () const;
  const glm::vec3& position () const;
  const glm::vec3& normal () const;
  glm::vec3        delta () const;
  PrimSphere       sphere () const;
  float            stepWidth () const;
  bool             hasPointOfAction () const;
  void             setPointOfAction (DynamicMesh&, const glm::vec3&, const glm::vec3&);
  void             resetPointOfAction ();
  void             mirror (const PrimPlane&);

  DynamicFaces getAffectedFaces () const;
  void         sculpt (const DynamicFaces&) const;

  template <typename T> T& initParameters ()
  {
    this->parametersPointer (new T ());
    return this->parameters<T> ();
  }

  template <typename T> T& parameters () { return dynamic_cast<T&> (*this->parametersPointer ()); }

  const SBParameters& parameters () const { return *this->parametersPointer (); }

private:
  SBParameters* parametersPointer () const;
  void          parametersPointer (SBParameters*);

  IMPLEMENTATION
};

#endif

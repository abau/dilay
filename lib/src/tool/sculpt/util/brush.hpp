/* This file is part of Dilay
 * Copyright © 2015,2016 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_TOOL_SCULPT_BRUSH
#define DILAY_TOOL_SCULPT_BRUSH

#include <glm/glm.hpp>
#include "macro.hpp"

class DynamicFaces;
class DynamicMesh;
class PrimPlane;
class PrimSphere;
class SculptBrush;

#define BRUSH_PARAMETER(type, name) \
public:                             \
  type name () const                \
  {                                 \
    return this->_##name;           \
  }                                 \
  void name (type v)                \
  {                                 \
    this->_##name = v;              \
  }                                 \
                                    \
private:                            \
  type _##name;

class SBParameters
{
public:
  virtual ~SBParameters ()
  {
  }

  virtual float intensity () const
  {
    return 0.0f;
  }

  virtual void intensity (float)
  {
  }

  virtual bool useLastPos () const
  {
    return false;
  }

  virtual bool discardBack () const
  {
    return true;
  }

  virtual bool reduce () const
  {
    return false;
  }

  virtual void sculpt (const SculptBrush&, const DynamicFaces&) const = 0;
};

class SBIntensityParameter : virtual public SBParameters
{
public:
  SBIntensityParameter ()
    : _intensity (0.0f)
  {
  }

  BRUSH_PARAMETER (float, intensity);
};

class SBInvertParameter : virtual public SBParameters
{
public:
  SBInvertParameter ()
    : _invert (false)
  {
  }

  void toggleInvert ()
  {
    this->_invert = !this->_invert;
  }

  glm::vec3 invert (const glm::vec3& v) const
  {
    return this->_invert ? -v : v;
  }

  BRUSH_PARAMETER (bool, invert);
};

class SBDrawParameters : public SBIntensityParameter, public SBInvertParameter
{
public:
  SBDrawParameters ()
    : _flat (true)
  {
  }

  void sculpt (const SculptBrush&, const DynamicFaces&) const;

  BRUSH_PARAMETER (bool, flat);
  BRUSH_PARAMETER (bool, constantHeight);
};

class SBGrablikeParameters : public SBParameters
{
public:
  SBGrablikeParameters ()
    : _discardBack (true)
  {
  }

  void sculpt (const SculptBrush&, const DynamicFaces&) const;

  bool useLastPos () const override
  {
    return true;
  }

  BRUSH_PARAMETER (bool, discardBack);
};

class SBSmoothParameters : public SBIntensityParameter
{
public:
  SBSmoothParameters ()
    : _relaxOnly (false)
  {
  }

  void sculpt (const SculptBrush&, const DynamicFaces&) const;

  BRUSH_PARAMETER (bool, relaxOnly);
};

class SBReduceParameters : public SBIntensityParameter
{
public:
  bool reduce () const override
  {
    return true;
  }

  void sculpt (const SculptBrush&, const DynamicFaces&) const;
};

class SBFlattenParameters : public SBIntensityParameter
{
public:
  void sculpt (const SculptBrush&, const DynamicFaces&) const;
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

#undef BRUSH_PARAMETER

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

  template <typename T> T& parameters ()
  {
    return dynamic_cast<T&> (*this->parametersPointer ());
  }

  const SBParameters& parameters () const
  {
    return *this->parametersPointer ();
  }

private:
  SBParameters* parametersPointer () const;
  void          parametersPointer (SBParameters*);

  IMPLEMENTATION
};

#endif

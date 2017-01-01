/* This file is part of Dilay
 * Copyright © 2015,2016 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_SCULPT_BRUSH
#define DILAY_SCULPT_BRUSH

#include <glm/glm.hpp>
#include "winged/fwd.hpp"
#include "macro.hpp"

class AffectedFaces;
class PrimPlane;
class PrimSphere;
class SculptBrush;

class SBParameters {
  public:
    virtual ~SBParameters () {}

    virtual float intensity  () const { return 0.0f; }
    virtual void  intensity  (float)  {}

    virtual bool  useLastPos  () const { return false; }
    virtual bool  discardBack () const { return true;  }
    virtual bool  reduce      () const { return false; }

    virtual void  sculpt (const SculptBrush&, const VertexPtrSet&) const = 0;
};

class SBIntensityParameter : virtual public SBParameters {
  public:
    SBIntensityParameter () : _intensity (0.0f) {}

    float intensity () const override  { return this->_intensity; }
    void  intensity (float v) override { this->_intensity = v; }

  private:
    float _intensity;
};

class SBInvertParameter : virtual public SBParameters {
  public:
    SBInvertParameter () : _invert (false) {}

    bool      invert       () const                   { return this->_invert; }
    void      invert       (bool v)                   { this->_invert = v; }
    void      toggleInvert ()                         { this->_invert = ! this->_invert; }
    glm::vec3 invert       (const glm::vec3& v) const { return this->_invert ? -v : v; }

  private:
    bool _invert;
};

class SBCarveParameters : public SBIntensityParameter, public SBInvertParameter {
  public:
    SBCarveParameters () : _inflate (false) {}

    bool inflate () const { return this->_inflate; }
    void inflate (bool v) { this->_inflate = v; }

    void sculpt (const SculptBrush&, const VertexPtrSet&) const;

  private:
    bool _inflate;
};

class SBDraglikeParameters : public SBParameters {
  public:
    SBDraglikeParameters ()
      : _smoothness  (1.0f)
      , _discardBack (true)
      , _linearStep  (false)
    {}

    float smoothness  () const          { return this->_smoothness; }
    bool  discardBack () const override { return this->_discardBack; }
    bool  linearStep  () const          { return this->_linearStep; }
    bool  useLastPos  () const override { return true; }

    void  smoothness  (float v) { this->_smoothness  = v; }
    void  discardBack (bool v)  { this->_discardBack = v; }
    void  linearStep  (bool v)  { this->_linearStep  = v; }

    void  sculpt (const SculptBrush&, const VertexPtrSet&) const;

  private:
    float _smoothness;
    bool  _discardBack;
    bool  _linearStep;
};

class SBSmoothParameters : public SBIntensityParameter {
  public:
    SBSmoothParameters () : _relaxOnly (false) {}

    bool relaxOnly () const { return this->_relaxOnly; }
    void relaxOnly (bool v) { this->_relaxOnly = v; }

    void sculpt (const SculptBrush&, const VertexPtrSet&) const;

  private:
    bool  _relaxOnly;
};

class SBReduceParameters  : public SBIntensityParameter {
  public:
    bool reduce () const override { return true; }

    void sculpt (const SculptBrush&, const VertexPtrSet&) const;
};

class SBFlattenParameters : public SBIntensityParameter {
  public:
    void sculpt (const SculptBrush&, const VertexPtrSet&) const;
};

class SBCreaseParameters : public SBIntensityParameter, public SBInvertParameter {
  public:
    void sculpt (const SculptBrush&, const VertexPtrSet&) const;
};

class SBPinchParameters : public SBInvertParameter {
  public:
    void sculpt (const SculptBrush&, const VertexPtrSet&) const;
};

class SculptBrush {
  public:
    DECLARE_BIG3 (SculptBrush)

    float               radius              () const;
    float               detailFactor        () const;
    float               stepWidthFactor     () const;
    bool                subdivide           () const;
    WingedMesh*         mesh                () const;

    void                radius              (float);
    void                detailFactor        (float);
    void                stepWidthFactor     (float);
    void                subdivide           (bool);
    void                mesh                (WingedMesh*);

    float               subdivThreshold     () const;
    bool                hasPosition         () const;
    const glm::vec3&    lastPosition        () const;
    const glm::vec3&    position            () const;
    const glm::vec3&    direction           () const;
    glm::vec3           delta               () const;
    PrimSphere          sphere              () const;
    void                setPointOfAction    (const glm::vec3&, const glm::vec3&);
    bool                updatePointOfAction (const glm::vec3&, const glm::vec3&);
    void                resetPointOfAction  ();
    void                mirror              (const PrimPlane&);

    AffectedFaces       getAffectedFaces    () const;
    void                sculpt              (const VertexPtrSet&) const;

    template <typename T>
    T& initParameters () {
      this->parametersPointer (new T ());
      return this->parameters <T> ();
    }

    template <typename T>
    T& parameters () {
      return dynamic_cast <T&> (*this->parametersPointer ());
    }

    const SBParameters& parameters () const {
      return *this->parametersPointer ();
    }

    SAFE_REF_CONST (WingedMesh, mesh)
  private:
    SBParameters* parametersPointer () const;
    void          parametersPointer (SBParameters*);

    IMPLEMENTATION
};

#endif

/* This file is part of Dilay
 * Copyright © 2015,2016 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_SCULPT_BRUSH
#define DILAY_SCULPT_BRUSH

#include <glm/glm.hpp>
#include "macro.hpp"

class AffectedFaces;
class PrimPlane;
class WingedFace;
class WingedMesh;

class SBIntensityParameters {
  public:
    SBIntensityParameters ();

    float intensity () const  { return this->_intensity; }
    void  intensity (float v) { this->_intensity = v; }

  private:
    float _intensity;
};

class SBInvertParameters {
  public:
    SBInvertParameters ();

    bool      invert       () const { return this->_invert; }
    void      invert       (bool v) { this->_invert = v; }
    void      toggleInvert ()       { this->_invert = ! this->_invert; }
    glm::vec3 invert       (const glm::vec3&) const;

  private:
    bool _invert;
};


class SBCarveParameters : public SBIntensityParameters, public SBInvertParameters {
  public:
    SBCarveParameters ();

    bool inflate () const { return this->_inflate; }
    void inflate (bool v) { this->_inflate = v; }

  private:
    bool _inflate;
};

class SBDraglikeParameters {
  public:
    SBDraglikeParameters ();

    float smoothness       () const { return this->_smoothness; }
    bool  discardBackfaces () const { return this->_discardBackfaces; }
    bool  linearStep       () const { return this->_linearStep; }

    void  smoothness       (float v) { this->_smoothness       = v; }
    void  discardBackfaces (bool v)  { this->_discardBackfaces = v; }
    void  linearStep       (bool v)  { this->_linearStep       = v; }

  private:
    float _smoothness;
    bool  _discardBackfaces;
    bool  _linearStep;
};

class SBSmoothParameters : public SBIntensityParameters {
  public:
    SBSmoothParameters ();

    bool relaxOnly () const { return this->_relaxOnly; }
    void relaxOnly (bool v) { this->_relaxOnly = v; }

  private:
    bool  _relaxOnly;
};

class SBFlattenParameters : public SBIntensityParameters {};

class SBCreaseParameters : public SBIntensityParameters, public SBInvertParameters {};
class SBPinchParameters : public SBInvertParameters {};
class SBReduceParameters : public SBIntensityParameters {};

class SculptBrush {
  public:
    DECLARE_BIG6 (SculptBrush)

    void             sculpt              (AffectedFaces&) const;

    float            radius              () const;
    float            detailFactor        () const;
    float            stepWidthFactor     () const;
    bool             subdivide           () const;
    WingedMesh*      mesh                () const;

    void             radius              (float);
    void             detailFactor        (float);
    void             stepWidthFactor     (float);
    void             subdivide           (bool);
    void             mesh                (WingedMesh*);

    float            subdivThreshold     () const;
    bool             hasPosition         () const;
    const glm::vec3& lastPosition        () const;
    const glm::vec3& position            () const;
    const glm::vec3& direction           () const;
    glm::vec3        delta               () const;
    void             setPointOfAction    (const glm::vec3&, const glm::vec3&);
    bool             updatePointOfAction (const glm::vec3&, const glm::vec3&);
    void             resetPointOfAction  ();
    bool             reduce              () const;
    void             mirror              (const PrimPlane&);

    template <typename T> const T& constParameters () const;
    template <typename T>       T& parameters      ();

    SAFE_REF_CONST (WingedMesh, mesh)
  private:
    IMPLEMENTATION
};

#endif

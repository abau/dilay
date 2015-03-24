#ifndef DILAY_SCULPT_BRUSH
#define DILAY_SCULPT_BRUSH

#include <glm/glm.hpp>
#include "macro.hpp"

class AffectedFaces;
class Mirror;
class WingedFace;
class WingedMesh;

class SBMoveDirectionalParameters {
  public:
    SBMoveDirectionalParameters ();

    float intensityFactor  () const  { return this->_intensityFactor; }
    float smoothness       () const  { return this->_smoothness; }
    bool  invert           () const  { return this->_invert; }
    bool  useAverageNormal () const  { return this->_useAverageNormal; }
    bool  useLastPosition  () const  { return this->_useLastPosition; }
    bool  discardBackfaces () const  { return this->_discardBackfaces; }
    bool  linearStep       () const  { return this->_linearStep; }

    void  intensityFactor  (float v) { this->_intensityFactor = v; }
    void  smoothness       (float v) { this->_smoothness = v; }
    void  invert           (bool v)  { this->_invert = v; }
    void  useAverageNormal (bool v)  { this->_useAverageNormal = v; }
    void  useLastPosition  (bool v)  { this->_useLastPosition = v; }
    void  discardBackfaces (bool v)  { this->_discardBackfaces = v; }
    void  linearStep       (bool v)  { this->_linearStep = v; }

    void  toggleInvert     ()        { this->_invert = ! this->_invert; }

  private:
    float     _intensityFactor;
    float     _smoothness;
    bool      _invert;
    bool      _useAverageNormal;
    bool      _useLastPosition;
    bool      _discardBackfaces;
    bool      _linearStep;
};

class SBSmoothParameters {
  public:
    SBSmoothParameters ();

    bool  relaxOnly () const { return this->_relaxOnly; }
    float intensity () const { return this->_intensity; }

    void  relaxOnly (bool v)  { this->_relaxOnly = v; }
    void  intensity (float v) { this->_intensity = v; }

  private:
    bool  _relaxOnly;
    float _intensity;
};

class SBFlattenParameters {
  public:
    SBFlattenParameters ();

    float intensity () const  { return this->_intensity; }
    void  intensity (float v) { this->_intensity = v; }

  private:
    float _intensity;
};

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
    void             mirror              (const Mirror&);

    template <typename T> const T& constParameters () const;
    template <typename T>       T& parameters      ();

    SAFE_REF_CONST (WingedMesh, mesh)
  private:
    IMPLEMENTATION
};

#endif

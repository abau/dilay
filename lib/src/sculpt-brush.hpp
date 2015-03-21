#ifndef DILAY_SCULPT_BRUSH
#define DILAY_SCULPT_BRUSH

#include <glm/glm.hpp>
#include "macro.hpp"

class AffectedFaces;
class WingedFace;
class WingedMesh;

class SBMoveDirectionalParameters {
  public:
    SBMoveDirectionalParameters ();

    float            intensityFactor     () const { return this->_intensityFactor; }
    float            innerRadiusFactor   () const { return this->_innerRadiusFactor; }
    bool             invert              () const { return this->_invert; }
    const glm::vec3& direction           () const { return this->_direction; }
    bool             useAverageDirection () const { return this->_useAverageDirection; }
    bool             useLastPosition     () const { return this->_useLastPosition; }
    bool             useIntersection     () const { return this->_useIntersection; }
    bool             linearStep          () const { return this->_linearStep; }

    void intensityFactor     (float v)            { this->_intensityFactor = v; }
    void innerRadiusFactor   (float v)            { this->_innerRadiusFactor = v; }
    void invert              (bool v)             { this->_invert = v; }
    void direction           (const glm::vec3& v) { this->_direction = v
                                                  ; this->_useAverageDirection = false;
                                                  }
    void useAverageDirection (bool v)             { this->_useAverageDirection = v; }
    void useLastPosition     (bool v)             { this->_useLastPosition = v; }
    void useIntersection     (bool v)             { this->_useIntersection = v; }
    void linearStep          (bool v)             { this->_linearStep = v; }

    void toggleInvert        ()                   { this->_invert = ! this->_invert; }

  private:
    float     _intensityFactor;
    float     _innerRadiusFactor;
    bool      _invert;
    glm::vec3 _direction;
    bool      _useAverageDirection;
    bool      _useLastPosition;
    bool      _useIntersection;
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
    WingedFace*      face                () const;

    void             radius              (float);
    void             detailFactor        (float);
    void             stepWidthFactor     (float);
    void             subdivide           (bool);
    void             mesh                (WingedMesh*);
    void             face                (WingedFace*);

    float            subdivThreshold     () const;
    bool             hasPosition         () const;
    const glm::vec3& lastPosition        () const;
    const glm::vec3& position            () const;
    glm::vec3        delta               () const;
    void             setPosition         (const glm::vec3&);
    bool             updateDelta         (const glm::vec3&);
    bool             updatePosition      (const glm::vec3&);
    void             resetPosition       ();

    template <typename T> const T& constParameters () const;
    template <typename T>       T& parameters      ();

    SAFE_REF_CONST (WingedMesh, mesh)
    SAFE_REF_CONST (WingedFace, face)
  private:
    IMPLEMENTATION
};

#endif

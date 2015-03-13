#ifndef DILAY_VIEW_CURSOR
#define DILAY_VIEW_CURSOR

#include <glm/fwd.hpp>
#include "macro.hpp"

class Camera;
class Color;
class Mesh;

class ViewCursor {
  public: 
    DECLARE_BIG6 (ViewCursor)

          float        radius            () const;
    const Color&       color             () const;
          glm::vec3    position          () const;
    const glm::mat4x4& rotationMatrix    () const;
          bool         isEnabled         () const;
          bool         hasInnerRadius    () const;
          float        innerRadiusFactor () const;

    void  radius            (float);
    void  color             (const Color&);
    void  position          (const glm::vec3&);
    void  normal            (const glm::vec3&);
    void  enable            ();
    void  disable           ();
    void  hasInnerRadius    (bool);
    void  innerRadiusFactor (float);

    void  render            (Camera&) const;

  private:
    IMPLEMENTATION
};

#endif

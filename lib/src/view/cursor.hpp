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

          float      radius            () const;
          glm::vec3  position          () const;
    const glm::vec3& normal            () const;
    const Color&     color             () const;
          bool       isEnabled         () const;
          bool       hasInnerRadius    () const;
          float      innerRadiusFactor () const;

    void  radius            (float);
    void  position          (const glm::vec3&);
    void  normal            (const glm::vec3&);
    void  color             (const Color&);
    void  enable            ();
    void  disable           ();
    void  hasInnerRadius    (bool);
    void  innerRadiusFactor (float);

    void  render            (Camera&) const;

  private:
    IMPLEMENTATION
};

#endif

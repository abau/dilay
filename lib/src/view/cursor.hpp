#ifndef DILAY_VIEW_CURSOR
#define DILAY_VIEW_CURSOR

#include <glm/fwd.hpp>
#include "macro.hpp"

class Camera;
class Color;

class ViewCursor {
  public: 
    DECLARE_BIG6 (ViewCursor, float, const Color&)

    float        radius   () const;
    const Color& color    () const;
    glm::vec3    position () const;

    void  radius         (float);
    void  position       (const glm::vec3&);
    void  normal         (const glm::vec3&);
    void  color          (const Color&);
    void  render         (const Camera&) const;
    void  enable         ();
    void  disable        ();
    bool  isEnabled      () const;

  private:
    IMPLEMENTATION
};

#endif

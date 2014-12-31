#ifndef DILAY_VIEW_CURSOR
#define DILAY_VIEW_CURSOR

#include <glm/fwd.hpp>
#include "macro.hpp"

class Camera;

class ViewCursor {
  public: 
    DECLARE_BIG6 (ViewCursor, float)

    void  radius         (float);
    void  position       (const glm::vec3&);
    void  normal         (const glm::vec3&);
    void  updateGeometry ();
    void  render         (const Camera&);
    void  enable         ();
    void  disable        ();
    bool  isEnabled      () const;
    float radius         () const;

  private:
    IMPLEMENTATION
};

#endif

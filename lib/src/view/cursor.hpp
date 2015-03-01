#ifndef DILAY_VIEW_CURSOR
#define DILAY_VIEW_CURSOR

#include <glm/fwd.hpp>
#include "macro.hpp"

class Camera;
class Color;
class Mesh;

class ViewCursor {
  public: 
    DECLARE_BIG6_VIRTUAL (ViewCursor)

          float        radius         () const;
    const Color&       color          () const;
          glm::vec3    position       () const;
    const glm::mat4x4& rotationMatrix () const;

    void  radius         (float);
    void  color          (const Color&);
    void  position       (const glm::vec3&);
    void  normal         (const glm::vec3&);
    void  render         (Camera&) const;
    void  enable         ();
    void  disable        ();
    bool  isEnabled      () const;

  protected:
    static void updateCircleGeometry (Mesh&, float);

  private:
    IMPLEMENTATION

    virtual void runUpdate ()              {}
    virtual void runRender (Camera&) const {}
};

#endif

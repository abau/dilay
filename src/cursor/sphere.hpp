#include <glm/glm.hpp>
#include "util.hpp"
#include "mesh.hpp"

#ifndef CURSOR_SPHERE
#define CURSOR_SPHERE

class CursorSphere {
  public:
         CursorSphere (float);
    void setPosition  (const glm::vec3& v) { this->mesh.translate (v); }
    void render       ();

  private:
    PRIVATE_ASSIGNMENT_OP(CursorSphere)

    Mesh       mesh;
    static int rings;
    static int sectors;
};

#endif

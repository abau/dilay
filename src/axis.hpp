#include <glm/glm.hpp>
#include "util.hpp"
#include "mesh.hpp"

#ifndef AXIS
#define AXIS

class Axis {
  public:
         Axis       ();
    void initialize ();
    void render     ();

  private:
    PRIVATE_ASSIGNMENT_OP(Axis)

    Mesh  mesh;
    float length;
};

#endif

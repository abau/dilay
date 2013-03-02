#include <glm/glm.hpp>
#include "util.hpp"
#include "mesh.hpp"

#ifndef AXIS
#define AXIS

class Axis {
  public: Axis       ();
          Axis       (const Axis&) = delete;
    Axis& operator=  (const Axis&) = delete;

    void  initialize ();
    void  render     ();

  private:
    Mesh   mesh;
    float  length;
};

#endif

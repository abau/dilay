#ifndef DILAY_PRIMITIVE_SPHERE
#define DILAY_PRIMITIVE_SPHERE

#include <glm/glm.hpp>
#include <iosfwd>

class PrimSphere {
  public:
    PrimSphere (const glm::vec3&, float);

    const glm::vec3& center () const { return this->_center; }
    float            radius () const { return this->_radius; }

  private:
    const glm::vec3 _center;
    const float     _radius;
};

std::ostream& operator<<(std::ostream&, const PrimSphere&);

#endif

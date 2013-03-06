#ifndef RAY
#define RAY

#include <sstream>
#include <glm/glm.hpp>

class Ray {
  public:
    Ray (const glm::vec3& o, const glm::vec3& d) : _origin    (o)
                                                 , _direction (glm::normalize (d)) {}

    const glm::vec3& origin    () const { return this->_origin; }
    const glm::vec3& direction () const { return this->_direction; }

    glm::vec3 pointAt (float t) const {
      return this->_origin + (this->_direction * glm::vec3 (t));
    }

  private:
    const glm::vec3 _origin;
    const glm::vec3 _direction;
};

std::ostream& operator<<(std::ostream&, const Ray&);

#endif

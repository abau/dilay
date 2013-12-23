#ifndef DILAY_SPHERE_JOINT
#define DILAY_SPHERE_JOINT

#include <glm/glm.hpp>
#include "id.hpp"

class SphereJoint {
  public: 
    SphereJoint (const glm::vec3&, float);

    Id               id       () const { return this->_id.id (); }
    const glm::vec3& position () const { return this->_position; }
    float            radius   () const { return this->_radius; }
    
    void             position (const glm::vec3& p) { this->_position = p; }
    void             radius   (float r)            { this->_radius   = r; }

  private:
    const IdObject  _id;
    glm::vec3       _position;
    float           _radius;
};

#endif

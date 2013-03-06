#ifndef CURSOR
#define CURSOR

#include <glm/glm.hpp>
#include "util.hpp"
#include "mesh.hpp"

class Cursor {
  public: Cursor () : _radius (0.2f), _sectors (20), _isEnabled (false) {}
          Cursor (const Cursor&) = delete;

    void  initialize   ();
    void  setPosition  (const glm::vec3& v) { this->_mesh.setPosition (v); }
    void  setNormal    (const glm::vec3&);
    void  render       ();
    void  enable       () { this->_isEnabled = true;  }
    void  disable      () { this->_isEnabled = false; }
    bool  isEnabled    () const { return this->_isEnabled; }

  private:
    Mesh          _mesh;
    float         _radius;
    unsigned int  _sectors;
    bool          _isEnabled;
};

#endif

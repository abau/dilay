#ifndef DILAY_CURSOR
#define DILAY_CURSOR

#include <glm/fwd.hpp>
#include "macro.hpp"

class Cursor {
  public: 
    DECLARE_BIG6 (Cursor)

    void  setGeometry        (float);
    void  position           (const glm::vec3&);
    void  normal             (const glm::vec3&);
    void  render             ();
    void  enable             ();
    void  disable            ();
    bool  isEnabled          () const;
    float radius             () const;

  private:
    IMPLEMENTATION
};

#endif

#ifndef DILAY_CURSOR
#define DILAY_CURSOR

#include <glm/fwd.hpp>
#include "macro.hpp"

class Cursor {
  public: 
    DECLARE_BIG6 (Cursor)

    void  initialize         ();
    void  setPosition        (const glm::vec3&);
    void  setNormal          (const glm::vec3&);
    void  render             ();
    void  enable             ();
    void  disable            ();
    bool  isEnabled          () const;
    float radius             () const;

  private:
    class Impl;
    Impl* impl;
};

#endif

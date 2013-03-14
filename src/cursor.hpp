#ifndef DILAY_CURSOR
#define DILAY_CURSOR

#include "fwd-glm.hpp"

class CursorImpl;
class Mesh;

class Cursor {
  public:  Cursor            ();
           Cursor            (const Cursor&);
           Cursor& operator= (const Cursor&);
          ~Cursor            ();

    void  initialize   ();
    void  setPosition  (const glm::vec3&);
    void  setNormal    (const glm::vec3&);
    void  render       ();
    void  enable       ();
    void  disable      ();
    bool  isEnabled    () const;

  private:
    CursorImpl* impl;
};

#endif

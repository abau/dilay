#ifndef DILAY_COLOR
#define DILAY_COLOR

#include <iosfwd>
#include "macro.hpp"
#include "fwd-glm.hpp"

class Color {
  public: 
    DECLARE4_BIG5  (Color,float,float,float,float)
    Color          ();
    Color          (float,float,float);
    Color          (const glm::vec3&);
    Color          (const glm::vec4&);

    float r        () const;
    float g        () const;
    float b        () const;
    float opacity  () const;

    glm::vec3 vec3 () const;
    glm::vec4 vec4 () const;

  private:
    class Impl;
    Impl* impl;
};

std::ostream& operator<<(std::ostream&, const Color&);

#endif

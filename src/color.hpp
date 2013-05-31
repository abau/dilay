#ifndef DILAY_COLOR
#define DILAY_COLOR

#include <iosfwd>
#include "fwd-glm.hpp"

class ColorImpl;

class Color {
  public: 
          Color            (float,float,float,float);
          Color            (float,float,float);
          Color            (const glm::vec3&);
          Color            (const glm::vec4&);
          Color            (const Color&);
          Color            ();
    const Color& operator= (const Color&);
         ~Color            ();

    float r       () const;
    float g       () const;
    float b       () const;
    float opacity () const;

    glm::vec3 vec3 () const;
    glm::vec4 vec4 () const;

  private:
    ColorImpl* impl;
};

std::ostream& operator<<(std::ostream&, const Color&);

#endif

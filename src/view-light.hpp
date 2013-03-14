#ifndef DILAY_VIEW_LIGHT
#define DILAY_VIEW_LIGHT

#include "fwd-glm.hpp"

class ViewLightImpl;
class Color;

class ViewLight {
  public:
     ViewLight            (const glm::vec3&, const Color&, float); 
     ViewLight            (const ViewLight&);
     ViewLight& operator= (const ViewLight&);
    ~ViewLight            (); 

    const glm::vec3& position   () const;
    const Color&     color      () const;
          float      irradiance () const;

    void position   (const glm::vec3&);
    void color      (const Color&);
    void irradiance (float);
  private:
    ViewLightImpl* impl;
};

#endif

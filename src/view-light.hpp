#ifndef VIEW_LIGHT
#define VIEW_LIGHT

class ViewLight {
  public:
    ViewLight (const glm::vec3& p, const glm::vec3& c, float i) 
      : _position   (p)
      , _color      (c)
      , _irradiance (i)
      {}

    const glm::vec3& position   () const { return this->_position; }
    const glm::vec3& color      () const { return this->_color; }
          float      irradiance () const { return this->_irradiance; }

    void setPosition   (const glm::vec3 p) { this->_position   = p; }
    void setColor      (const glm::vec3 c) { this->_color      = c; }
    void setIrradiance (float i)           { this->_irradiance = i; }
  private:
    glm::vec3 _position;
    glm::vec3 _color;
    float     _irradiance;
};

#endif

#ifndef DILAY_MIRROR
#define DILAY_MIRROR

#include <glm/fwd.hpp>
#include "configurable.hpp"
#include "macro.hpp"

class Camera;
enum class Dimension;

class Mirror : public Configurable {
  public:
    DECLARE_BIG6 (Mirror, const Config&, Dimension)

    Dimension dimension () const;

    void      dimension (Dimension);
    glm::vec3 mirror    (const glm::vec3&) const;
    void      render    (Camera&) const;

  private:
    IMPLEMENTATION

    void runFromConfig (const Config&);
};

#endif

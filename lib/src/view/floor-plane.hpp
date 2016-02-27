/* This file is part of Dilay
 * Copyright © 2015,2016 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_VIEW_FLOORPLANE
#define DILAY_VIEW_FLOORPLANE

#include "configurable.hpp"
#include "macro.hpp"

class Camera;
class Config;

class ViewFloorPlane : public Configurable {
  public:
    DECLARE_BIG3 (ViewFloorPlane, const Config&, const Camera&)

    void render   (Camera&) const;
    void update   (const Camera&);
    bool isActive () const;
    void isActive (bool);

  private:
    IMPLEMENTATION

    void runFromConfig (const Config&);
};

#endif

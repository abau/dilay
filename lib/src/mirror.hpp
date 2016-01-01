/* This file is part of Dilay
 * Copyright © 2015,2016 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_MIRROR
#define DILAY_MIRROR

#include "configurable.hpp"
#include "macro.hpp"

class Camera;
enum class Dimension;
class PrimPlane;

class Mirror : public Configurable {
  public:
    DECLARE_BIG2 (Mirror, const Config&, Dimension)

    Dimension        dimension () const;
    float            width     () const;
    const PrimPlane& plane     () const;

    void             dimension (Dimension);
    void             render    (Camera&) const;

  private:
    IMPLEMENTATION

    void runFromConfig (const Config&);
};

#endif

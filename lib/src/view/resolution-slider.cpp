/* This file is part of Dilay
 * Copyright © 2015-2018 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include "view/resolution-slider.hpp"
#include "view/util.hpp"

struct ViewResolutionSlider::Impl
{
  Impl (ViewResolutionSlider* s, float min, float max)
  {
    ViewDoubleSlider* doubleSlider = s;
    ViewUtil::connect (*doubleSlider,
                       [s, min, max](float r) { emit s->resolutionChanged (max + min - r); });
  }
};

DELEGATE_BIG2_BASE (ViewResolutionSlider, (float min, float max), (this, min, max),
                    ViewDoubleSlider, (2, 1))

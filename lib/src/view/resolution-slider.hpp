/* This file is part of Dilay
 * Copyright © 2015-2018 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_VIEW_RESOLUTION_SLIDER
#define DILAY_VIEW_RESOLUTION_SLIDER

#include "view/double-slider.hpp"

class ViewResolutionSlider : public ViewDoubleSlider
{
  Q_OBJECT

public:
  DECLARE_BIG2 (ViewResolutionSlider, float, float)

signals:
  void resolutionChanged (float);

private:
  IMPLEMENTATION
};

#endif

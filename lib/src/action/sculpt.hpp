/* This file is part of Dilay
 * Copyright © 2015,2016 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_ACTION_SCULPT
#define DILAY_ACTION_SCULPT

class SculptBrush;
class WingedMesh;

namespace Action {

  void sculpt     (const SculptBrush&);
  void smoothMesh (WingedMesh&);
};

#endif

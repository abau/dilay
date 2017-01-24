/* This file is part of Dilay
 * Copyright © 2015,2016 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_TIME_DELTA
#define DILAY_TIME_DELTA

#define TIME_DELTA(X) TimeDelta::resetTimer (); X ; TimeDelta::addBreakpoint (#X);

namespace TimeDelta {
  void initialize    ();
  void resetTimer    ();
  void addBreakpoint (const char*);
}

#endif

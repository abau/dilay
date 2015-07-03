/* This file is part of Dilay
 * Copyright © 2015 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_TIME_DELTA
#define DILAY_TIME_DELTA

#include "macro.hpp"

#define TIME_DELTA(t) TimeDelta t (__FILE__,__LINE__);

class TimeDelta {
  public: 
    DECLARE_BIG4COPY (TimeDelta)
    TimeDelta    (const char*,int);

    void printGlobal (const char* = nullptr) const;
    void printLocal  (const char* = nullptr);

  private:
    IMPLEMENTATION
};

#endif

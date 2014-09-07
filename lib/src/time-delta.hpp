#ifndef DILAY_TIME_DELTA
#define DILAY_TIME_DELTA

#include "macro.hpp"

#define TIME_DELTA(t) TimeDelta t (__FILE__,__LINE__);

class TimeDelta {
  public: 
    DECLARE_BIG6 (TimeDelta)
    TimeDelta    (const char*,int);

    void printGlobal (const char* = nullptr) const;
    void printLocal  (const char* = nullptr);

  private:
    IMPLEMENTATION
};

#endif

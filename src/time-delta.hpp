#ifndef DILAY_TIME_DELTA
#define DILAY_TIME_DELTA

#define TIME_DELTA(t) TimeDelta t (__FILE__,__LINE__);

class TimeDelta {
  public: TimeDelta            ();
          TimeDelta            (const char*,int);
          TimeDelta            (const TimeDelta&);
    const TimeDelta& operator= (const TimeDelta&);
         ~TimeDelta            ();

    void stopAndPrint          ();

  private:
    class Impl;
    Impl* impl;
};

#endif

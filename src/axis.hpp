#ifndef DILAY_AXIS
#define DILAY_AXIS

class Axis {
  public: Axis             ();
          ~Axis            ();
          Axis             (const Axis&) = delete;
    const Axis& operator=  (const Axis&) = delete;

    void  initialize ();
    void  render     ();

  private:
    class Impl;
    Impl* impl;
};

#endif

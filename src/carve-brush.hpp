#ifndef DILAY_CARVE_BRUSH
#define DILAY_CARVE_BRUSH

class CarveBrush {
  public:
    CarveBrush (float, float);

    float y      (float) const;
    float width  ()      const;
    float height ()      const;
  private:
    class Impl;
    Impl* impl;
};

#endif

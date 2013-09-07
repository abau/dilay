#ifndef DILAY_GAUSS
#define DILAY_GAUSS

class Gauss {
  public:
    Gauss (float,float);

    float y (float) const;
  private:
    const float alpha;
    const float tmp;
};

#endif

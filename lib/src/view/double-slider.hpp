#ifndef DILAY_DOUBLE_SLIDER
#define DILAY_DOUBLE_SLIDER

#include <QSlider>
#include "macro.hpp"

class ViewDoubleSlider : public QSlider {
  Q_OBJECT
  public:
    DECLARE_BIG2 (ViewDoubleSlider, unsigned short)

    double doubleValue      () const;
    double doubleSingleStep () const;

    void  setValue      (double);
    void  setRange      (double, double);
    void  setSingleStep (double);
    void  setPageStep   (double);

  signals:
    void doubleValueChanged (double);

  private:
    IMPLEMENTATION
};

#endif

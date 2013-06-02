#ifndef VIEW_MOUSE_MOVEMENT
#define VIEW_MOUSE_MOVEMENT

class QPoint;

class MouseMovement {
  public:
          MouseMovement            ();
          MouseMovement            (const MouseMovement&);
    const MouseMovement& operator= (const MouseMovement&);
         ~MouseMovement            ();

    void  update      (const QPoint&);
    void  invalidate  ();
    int   dX          () const;
    int   dY          () const;

  private:
    class Impl;
    Impl* impl;
};

#endif

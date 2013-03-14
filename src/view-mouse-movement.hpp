#ifndef VIEW_MOUSE_MOVEMENT
#define VIEW_MOUSE_MOVEMENT

class MouseMovementImpl;
class QPoint;

class MouseMovement {
  public:
     MouseMovement            ();
     MouseMovement            (const MouseMovement&);
     MouseMovement& operator= (const MouseMovement&);
    ~MouseMovement            ();

    void  update      (const QPoint&);
    void  invalidate  ();
    int   dX          () const;
    int   dY          () const;

  private:
    MouseMovementImpl* impl;
};

#endif

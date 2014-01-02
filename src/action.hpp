#ifndef DILAY_ACTION
#define DILAY_ACTION

class Action {
  public:
    virtual ~Action () {}

    virtual void undo () = 0;
    virtual void redo () = 0;
};

#endif

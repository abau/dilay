#ifndef DILAY_ACTION
#define DILAY_ACTION

class Action {
  public:
    virtual ~Action () {}

    void undo () { this->runUndo (); }
    void redo () { this->runRedo (); }

  private:
    virtual void runUndo () = 0;
    virtual void runRedo () = 0;
};

#endif

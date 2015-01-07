#ifndef DILAY_ACTION
#define DILAY_ACTION

class Action {
  public:
    virtual ~Action () {}

    void undo () const { this->runUndo (); }
    void redo () const { this->runRedo (); }

  private:
    virtual void runUndo () const = 0;
    virtual void runRedo () const = 0;
};

#endif

#ifndef DILAY_ACTION_ON
#define DILAY_ACTION_ON

template <typename T>
class ActionOn {
  public:
    virtual ~ActionOn () {}

    void undo (T& t) { this->runUndo (t); }
    void redo (T& t) { this->runRedo (t); }

  private:
    virtual void runUndo (T&) = 0;
    virtual void runRedo (T&) = 0;
};

#endif

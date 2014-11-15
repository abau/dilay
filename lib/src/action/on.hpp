#ifndef DILAY_ACTION_ON
#define DILAY_ACTION_ON

template <typename T>
class ActionOn {
  public:
    virtual ~ActionOn () {}

    void undo (T& t) const { this->runUndo (t); }
    void redo (T& t) const { this->runRedo (t); }

  private:
    virtual void runUndo (T&) const = 0;
    virtual void runRedo (T&) const = 0;
};

#endif

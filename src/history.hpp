#ifndef DILAY_HISTORY
#define DILAY_HISTORY

class Action;

class History {
  public: History            ();
          History            (const History&) = delete;
    const History& operator= (const History&) = delete;
         ~History            ();

    template <class T>
    T* add () { 
      T* action = new T ();
      this->addAction (action); 
      return action; 
    }

    void addAction (Action*);
    void reset     ();
    void undo      ();
    void redo      ();
  private:
    class Impl;
    Impl* impl;
};

#endif

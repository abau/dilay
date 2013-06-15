#ifndef DILAY_HISTORY
#define DILAY_HISTORY

class Action;

class History {
  public: History            ();
          History            (const History&);
    const History& operator= (const History&);
         ~History            ();

    template <class T>
    void add  (const T& action) {
      this->addAction (new T (action));
    }

    void undo      ();
    void redo      ();

  private:
    void addAction (Action*);

    class Impl;
    Impl* impl;
};

#endif

#ifndef DILAY_HISTORY
#define DILAY_HISTORY

class Action;

class History {
  public: History            ();
          History            (const History&) = delete;
    const History& operator= (const History&) = delete;
         ~History            ();

    template <class T>
    void addAction  (const T& action) {
      this->addActionPtr (new T (action));
    }

    void undo      ();
    void redo      ();

  private:
    void addActionPtr (Action*);

    class Impl;
    Impl* impl;
};

#endif

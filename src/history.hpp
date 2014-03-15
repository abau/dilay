#ifndef DILAY_HISTORY
#define DILAY_HISTORY

#include "action/transformer.hpp"

class Action;
class WingedMesh;

class History {
  public: History            ();
          History            (const History&) = delete;
    const History& operator= (const History&) = delete;
         ~History            ();

    template <typename T>
    T* add () { 
      T* action = new T ();
      this->addAction (action); 
      return action; 
    }

    template <typename T>
    T* add (WingedMesh& mesh) { 
      T* action = new T ();
      this->addAction (new ActionTransformer (mesh, action));
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

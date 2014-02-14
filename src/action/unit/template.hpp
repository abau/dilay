#ifndef DILAY_ACTION_UNIT_TEMPLATE
#define DILAY_ACTION_UNIT_TEMPLATE

#include <list>
#include <memory>
#include <functional>

template <class T>
class ActionUnitTemplate {
  public: 
          ActionUnitTemplate () : _actions () {}
          ActionUnitTemplate (const ActionUnitTemplate&) = delete;
          ActionUnitTemplate (ActionUnitTemplate&& other) : _actions (std::move (other.actions)) {}
    const ActionUnitTemplate& operator= (const ActionUnitTemplate&) = delete;
         ~ActionUnitTemplate () {}

    typedef std::unique_ptr <T> ActionPtr;

    void addAction (T* action) { 
      this->_actions.push_back (ActionPtr (action));
    }

    void forall (std::function <void (T&)> f) {
      for (auto it = this->_actions.begin (); it != this->_actions.end (); ++it) {
        f (**it);
      }
    }

    void forallReverse (std::function <void (T&)> f) {
      for (auto it = this->_actions.rbegin (); it != this->_actions.rend (); ++it) {
        f (**it);
      }
    }

    void                     reset      ()       { this->_actions.clear (); }
    std::list < ActionPtr >& actions    ()       { return this->_actions; }
    unsigned int             numActions () const { return this->_actions.size (); }

  private:
    std::list < ActionPtr > _actions;
};
#endif

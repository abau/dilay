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
          ActionUnitTemplate (ActionUnitTemplate&& other) 
            : _preActions  (std::move (other._preActions)) 
            , _actions     (std::move (other._actions)) 
            , _postActions (std::move (other._postActions)) 
            {}

    const ActionUnitTemplate& operator= (const ActionUnitTemplate&) = delete;
    const ActionUnitTemplate& operator= (ActionUnitTemplate&&) = delete;
         ~ActionUnitTemplate () {}

    typedef std::unique_ptr <T> ActionPtr;

    void addAction (T* action, SubActionKind kind = SubActionKind::Normal) { 
      switch (kind) {
        case SubActionKind::Pre:
          this->_preActions.push_back (ActionPtr (action));
          break;
        case SubActionKind::Normal:
          this->_actions.push_back (ActionPtr (action));
          break;
        case SubActionKind::Post:
          this->_postActions.push_back (ActionPtr (action));
          break;
      }
    }

    void forall (std::function <void (T&)> f) {
      for (auto it = this->_preActions.begin (); it != this->_preActions.end (); ++it) {
        f (**it);
      }
      for (auto it = this->_actions.begin (); it != this->_actions.end (); ++it) {
        f (**it);
      }
      for (auto it = this->_postActions.begin (); it != this->_postActions.end (); ++it) {
        f (**it);
      }
    }

    void forallReverse (std::function <void (T&)> f) {
      for (auto it = this->_preActions.begin (); it != this->_preActions.end (); ++it) {
        f (**it);
      }
      for (auto it = this->_actions.rbegin (); it != this->_actions.rend (); ++it) {
        f (**it);
      }
      for (auto it = this->_postActions.begin (); it != this->_postActions.end (); ++it) {
        f (**it);
      }
    }

    void reset () { 
      this->_preActions .clear ();
      this->_actions    .clear (); 
      this->_postActions.clear ();
    }

    std::list < ActionPtr >& preActions  () { return this->_preActions; }
    std::list < ActionPtr >& actions     () { return this->_actions; }
    std::list < ActionPtr >& postActions () { return this->_postActions; }

  private:
    std::list <ActionPtr> _preActions;
    std::list <ActionPtr> _actions;
    std::list <ActionPtr> _postActions;
};
#endif

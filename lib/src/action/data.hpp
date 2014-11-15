#ifndef DILAY_ACTION_DATA
#define DILAY_ACTION_DATA

#include <utility>
#include "action/identifier.hpp"
#include "variant.hpp"

enum class ActionDataType { Old, New };

template <typename ... Ts>
class ActionData {
  public:
    void identifier (const ActionIdentifier& i) { 
      assert (i.isSet ());
      this->_identifier = i; 
    }

    const ActionIdentifier& identifier () const { return this->_identifier; }

    template <typename T>
    void values (const T& oldValue, const T& newValue) {
      this->_oldValue.template set <T> (oldValue);
      this->_newValue.template set <T> (newValue);
    }

    void valueIdentifiers (const ActionIdentifier& oldValue, const ActionIdentifier& newValue) {
      this->template values <ActionIdentifier> (oldValue, newValue);
    }

    template <typename T>
    void value (const T& newValue) {
      this->_newValue.template set <T> (newValue);
    }

    template <typename T>
    const T& value (ActionDataType t) const {
      switch (t) { case ActionDataType::Old: return this->_oldValue.template get <T> ();
                   case ActionDataType::New: return this->_newValue.template get <T> ();
      }
      std::abort ();
    }

    const ActionIdentifier& valueIdentifier (ActionDataType t) const {
      return this->template value <ActionIdentifier> (t);
    }

    template <typename T>
    const T& value () const {
      assert (this->_oldValue.isSet () == false);
      return this->template value <T> (ActionDataType::New);
    }

    const ActionIdentifier& valueIdentifier () const {
      assert (this->_oldValue.isSet () == false);
      return this->template value <ActionIdentifier> (ActionDataType::New);
    }

  private:
    ActionIdentifier _identifier;
    Variant <Ts ...> _oldValue;
    Variant <Ts ...> _newValue;
};

#endif

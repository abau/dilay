#ifndef DILAY_ACTION_DATA
#define DILAY_ACTION_DATA

#include <utility>
#include "action/identifier.hpp"
#include "variant.hpp"

template <typename ... Ts>
class ActionData {
  public:
    enum class Type { Old, New };

    void identifier (const ActionIdentifier& i) { 
      assert (i.isSet ());
      this->_identifier = i; 
    }

    const ActionIdentifier& identifier () const { return this->_identifier; }

    void valueIdentifiers (const ActionIdentifier& oldValue, const ActionIdentifier& newValue) {
      this->_oldValue.template set <ActionIdentifier> (oldValue);
      this->_newValue.template set <ActionIdentifier> (newValue);
    }

    template <typename T>
    void values (const T& oldValue, const T& newValue) {
      this->_oldValue.template set <T> (oldValue);
      this->_newValue.template set <T> (newValue);
    }

    template <typename T>
    const T& value (Type t) const {
      switch (t) { case Type::Old: return this->_oldValue.template get <T> ();
                   case Type::New: return this->_newValue.template get <T> ();
      }
      std::abort ();
    }

  private:
    ActionIdentifier _identifier;
    Variant <Ts ...> _oldValue;
    Variant <Ts ...> _newValue;
};

#endif

#ifndef DILAY_ACTION_DATA
#define DILAY_ACTION_DATA

#include <utility>
#include "maybe.hpp"
#include "variant.hpp"

enum class ActionDataType { Old, New };

template <typename ... Ts>
class ActionData {
  public:
    void index (unsigned int i) { 
      this->_index = i; 
    }

    template <typename T>
    void index (const T& value) { 
      this->index (value.index ());
    }

    unsigned int index () const { 
      return this->_index.getRef (); 
    }

    template <typename T>
    void values (const T& oldValue, const T& newValue) {
      this->_oldValue.template set <T> (oldValue);
      this->_newValue.template set <T> (newValue);
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

    template <typename T>
    const T& value () const {
      assert (this->_oldValue.isSet () == false);
      return this->template value <T> (ActionDataType::New);
    }

  private:
    Maybe   <unsigned int> _index;
    Variant <Ts ...>       _oldValue;
    Variant <Ts ...>       _newValue;
};

#endif

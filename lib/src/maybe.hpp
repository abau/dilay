#ifndef DILAY_MAYBE
#define DILAY_MAYBE

#include <cassert>
#include <memory>

template <typename T>
class Maybe {
  static_assert (std::is_pointer   <T>::value == false, "Maybe does not support pointers");
  static_assert (std::is_reference <T>::value == false, "Maybe does not support references");

  public:
    Maybe () = default;

    Maybe (const T* v) 
      : value (bool (v) ? new T (*v) : nullptr)
      {}

    Maybe (const T& v) 
      : value (new T (v))
      {}

    Maybe (const Maybe <T>& o) 
      : value (o.isSet () ? new T (o.getRef ()) : nullptr)
      {}

    Maybe (Maybe <T>&&) = default;

    const Maybe <T>& operator= (const T* v) {
      this->value.reset (bool (v) ? new T (*v) : nullptr);
      return *this;
    }

    const Maybe <T>& operator= (const T& v) {
      this->value.reset (new T (v));
      return *this;
    }

    const Maybe <T>& operator= (const Maybe<T>& o) {
      if (this != &o) {
        this->value.reset (o.isSet () ? new T (o.getRef ()) : nullptr);
      }
      return *this;
    }

    Maybe <T>& operator= (Maybe<T>&&) = default;

    ~Maybe () = default;

    bool isSet () const {
      return bool (this->value); 
    }

    T* get () const {
      return this->isSet () ? this->value.get () : nullptr ;
    }

    T& getRef () const {
      assert (this->isSet ());
      return *this->value;
    }

    void set (const T& newValue) {
      this->value.reset (new T (newValue));
    }

  private:
    std::unique_ptr <T> value;
};

#endif

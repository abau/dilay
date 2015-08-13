/* This file is part of Dilay
 * Copyright © 2015 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
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
      : Maybe (&v)
    {}

    Maybe (const Maybe <T>& o) 
      : Maybe (o.get ())
    {}

    Maybe (Maybe <T>&&) = default;

    Maybe (std::unique_ptr <T>&& v)
      : value (std::move (v))
    {}

    template <typename ... Args>
    static Maybe <T> make (Args&& ... args) {
      return Maybe (std::make_unique <T> (std::forward <Args> (args) ...));
    }

    ~Maybe () = default;

    Maybe <T>& operator= (const T* v) {
      this->value.reset (bool (v) ? new T (*v) : nullptr);
      return *this;
    }

    Maybe <T>& operator= (const T& v) {
      return this->operator= (&v);
    }

    Maybe <T>& operator= (const Maybe<T>& o) {
      if (this != &o) {
        this->operator= (o.get ());
      }
      return *this;
    }

    Maybe <T>& operator= (Maybe<T>&&) = default;

    explicit operator bool () const {
      return this->hasValue ();
    }

    bool operator== (bool v) const {
      return this->operator bool () == v;
    }

    T& operator* () {
      assert (this->hasValue ());
      return *this->value;
    }

    const T& operator* () const {
      assert (this->hasValue ());
      return *this->value;
    }

    T* operator-> () {
      return this->hasValue () ? this->get () : nullptr ;
    }

    const T* operator-> () const {
      return this->hasValue () ? this->get () : nullptr ;
    }

    T* get () {
      return this->value.get ();
    }

    const T* get () const {
      return this->value.get ();
    }

    bool hasValue () const {
      return bool (this->value); 
    }

    void reset (T* newValue = nullptr) {
      this->value.reset (newValue);
    }

    void swap (Maybe <T>& o) {
      this->value.swap (o.value);
    }

  private:
    std::unique_ptr <T> value;
};

#endif

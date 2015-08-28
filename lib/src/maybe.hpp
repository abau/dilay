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

    explicit Maybe (T* v)
      : value (v)
    {}

    Maybe (const T& v) 
      : value (new T (v))
    {}

    Maybe (const Maybe <T>& o) 
      : value (o.hasValue () ? new T (*o) : nullptr)
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

    Maybe <T>& operator= (T* v) {
      if (this->get () != v) {
        this->value.reset (v);
      }
      return *this;
    }

    Maybe <T>& operator= (const T& v) {
      if (this->get () != &v) {
        this->value.reset (new T (v));
      }
      return *this;
    }

    Maybe <T>& operator= (const Maybe<T>& o) {
      if (this != &o) {
        this->value.reset (o.hasValue () ? new T (*o) : nullptr);
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
      return this->get ();
    }

    const T* operator-> () const {
      return this->get ();
    }

    T* get () {
      return this->value.get ();
    }

    const T* get () const {
      return this->value.get ();
    }

    T* release () {
      return this->value.release ();
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

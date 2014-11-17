#ifndef DILAY_MAYBE
#define DILAY_MAYBE

#include <cassert>
#include <memory>

template <typename T>
class Maybe {
  public:
    Maybe () = default;

    Maybe (const Maybe <T>& o) 
      : value (o.isSet () ? new T (o.getRef ()) : nullptr)
      {}

    Maybe (Maybe <T>&&) = default;

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

#ifndef DILAY_MAYBE
#define DILAY_MAYBE

#include <iosfwd>
#include <string>
#include <cassert>

template <class T>
class Maybe {
  public:
    Maybe ()           : _isDefined (false) {}
    Maybe (const T& t) : _isDefined (true), _data (t) {}
    Maybe (const Maybe<T>& t) {
      if (t.isDefined ()) {
        this->_data      = T (t.data ());
        this->_isDefined = true;
      }
      else {
        this->_isDefined = false;
      }
    }

    const Maybe<T>& operator= (const Maybe<T>& t) {
      if (this == &t) return *this;
      Maybe<T> tmp (t);
      std::swap (this->_isDefined, tmp._isDefined);
      std::swap (this->_data     , tmp._data);
      return *this;
    }

          T& operator*  ()       { return  this->data (); }
    const T& operator*  () const { return  this->data (); }
          T* operator-> ()       { return &this->data (); }
    const T* operator-> () const { return &this->data (); }

    ~Maybe () { this->_isDefined = false; }

    bool isDefined   () const { return this->_isDefined; }
    bool isUndefined () const { return ! this->isDefined (); }
    void reset       ()       { this->_isDefined = false; }

          T&   data  ()       { assert (this->isDefined ()); return this->_data; }
    const T&   data  () const { assert (this->isDefined ()); return this->_data; }

    void data (const T& t) {
      this->_isDefined = true;
      this->_data = t;
    }

    friend std::ostream& operator<<(std::ostream& os, const Maybe<T>& d) {
      if (d.isDefined ()) os << std::string ("Defined { ") << d.data () << " }" ;
      else                os << std::string ("Undefined");
      return os;
    }

  private:
    bool _isDefined;

    union {
      T _data;
    };
};

template <class T>
class MaybePtr {
  public:
    MaybePtr ()           : _data (nullptr)   {}
    MaybePtr (const T& t) : _data (new T (t)) {}
    MaybePtr (const Maybe<T>& t) {
      if (t.isDefined ()) {
        this->_data = new T (t.data ());
      }
      else {
        this->_data = nullptr;
      }
    }

    const Maybe<T>& operator= (const Maybe<T>& t) {
      if (this == &t)
        return *this;
      MaybePtr <T> tmp (t);
      std::swap (this->_data, tmp._data);
      return *this;
    }

          T& operator*  ()       { return *this->data (); }
    const T& operator*  () const { return *this->data (); }
          T* operator-> ()       { return  this->data (); }
    const T* operator-> () const { return  this->data (); }

    ~MaybePtr () { this->reset (); }

    bool isDefined   () const { return ! this->isUndefined (); }
    bool isUndefined () const { return this->_data == nullptr; }
    void reset       ()       { 
      if (this->isDefined ()) {
        delete this->_data;
        this->_data = nullptr;
      }
    }

          T*   data  ()       { assert (this->isDefined ()); return this->_data; }
    const T*   data  () const { assert (this->isDefined ()); return this->_data; }

    void data (const T& t) {
      this->reset ();
      this->_data = new T (t);
    }

    friend std::ostream& operator<<(std::ostream& os, const Maybe<T>& d) {
      if (d.isDefined ()) os << std::string ("Defined { ") << *d.data () << " }" ;
      else                os << std::string ("Undefined");
      return os;
    }

  private:
    T* _data;
};

#endif

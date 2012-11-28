#include <iostream>
#include "util.hpp"

#ifndef MAYBE
#define MAYBE

template <class T>
class Maybe {
  public:
    Maybe ()                   : _data (0)         {}
    Maybe (const T& d)         : _data (new T (d)) {}
    Maybe (const Maybe <T>& d) : _data (d.isNothing () ? 0 : new T (d.data ())) {}

    ~Maybe () { this->undefine (); }

    const Maybe<T>& operator=(const Maybe<T>& d) {
      if (this == &d) return d;
      Maybe <T> tmp (d);
      Util :: swap (this->_data, tmp._data);
      return *this;
    }

    friend std::ostream& operator<<(std::ostream& os, const Maybe<T>& d) {
      if (d.isDefined ()) os << "Defined { " << d.data () << " }" ;
      else                os << "Nothing";
      return os;
    }

    T&        data      ()           { return *this->_data; }
    const T&  data      () const     { return *this->_data; }

    bool      isNothing () const     { return this->_data == 0; }
    bool      isDefined () const     { return ! this->isNothing (); }

    void      define    (const T& d) {
      if (this->isNothing ())
        this->_data = new T (d);
      else
        (*this->_data) = d;
    }

    void      undefine  ()           { 
      if (this->isDefined ()) {
        delete (this->_data);
        this->_data = 0;
      }
    }

    static Maybe <T> nothing () { return Maybe (); }

  private:
    T* _data;
};
#endif

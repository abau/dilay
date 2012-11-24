#include <iostream>
#include "util.hpp"

#ifndef MAYBE
#define MAYBE

template <class T>
class Maybe {
  private:
    T* _data;

  public:
    Maybe ()                   : _data (0)         {}
    Maybe (const T& d)         : _data (new T (d)) {}
    Maybe (const Maybe <T>& d) : _data (d.isNothing () ? 0 : new T (d.data ())) {}

    ~Maybe () { if (this->isDefined ()) delete (this->_data); }

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

    T&   data      () const { return *this->_data; }
    bool isNothing () const { return this->_data == 0; }
    bool isDefined () const { return ! this->isNothing (); }

    static Maybe <T> nothing () { return Maybe (); }
};
#endif

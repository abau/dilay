#ifndef DILAY_MAYBE
#define DILAY_MAYBE

#include <iosfwd>
#include <string>

// Maybe ////////////////////
template <class T>
class Maybe {
  public:
    Maybe ()                   : _data (0)         {}
    Maybe (const T& d)         : _data (new T (d)) {}
    Maybe (const Maybe <T>& d) : _data (d.isNothing () ? 0 : new T (d.data ())) {}

    ~Maybe () { this->undefine (); }

    const Maybe<T>& operator=(const Maybe<T>& d) {
      if (this == &d) return *this;
      Maybe <T> tmp (d);
      T* tmpData  = tmp._data;
      tmp._data   = this->_data;
      this->_data = tmpData;
      return *this;
    }

    friend std::ostream& operator<<(std::ostream& os, const Maybe<T>& d) {
      if (d.isDefined ()) os << std::string ("Defined {) ") << d.data () << " }" ;
      else                os << std::string ("Nothing");
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

// MaybePtr /////////////////
template <class T>
class MaybePtr {
  public:
    MaybePtr ()                      : _ptr (0)  {}
    MaybePtr (T* d)                  : _ptr (d) {}
    MaybePtr (const MaybePtr <T>& d) : _ptr (d.isNothing () ? 0 : d._ptr) {}

    ~MaybePtr () { this->undefine (); }

    friend std::ostream& operator<<(std::ostream& os, const Maybe<T>& d) {
      if (d.isDefined ()) os << std::string("DefinedPtr { ") << *(d.ptr ()) << " }" ;
      else                os << std::string("NothingPtr");
      return os;
    }

    T*        ptr       ()           { return this->_ptr; }
    const T*  ptr       () const     { return this->_ptr; }

    bool      isNothing () const     { return this->_ptr == 0; }
    bool      isDefined () const     { return ! this->isNothing (); }

    void      define    (T* d)       { this->_ptr = d; }
    void      undefine  ()           { this->_ptr = 0; }

    static MaybePtr <T> nothing ()   { return MaybePtr (); }

  private:
    T* _ptr;
};
#endif

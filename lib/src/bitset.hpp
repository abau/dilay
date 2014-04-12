#ifndef DILAY_BITSET
#define DILAY_BITSET

#include <initializer_list>

template <typename T>
class Bitset {
  public:
    typedef short Index;

    Bitset () : _data (0) {}
    Bitset (std::initializer_list <Index> ones) : _data (0) {
      for (Index i : ones) {
        this->set (i);
      }
    }

    Bitset (std::initializer_list <bool> bits) : _data (0) {
      Index i = 0;
      for (bool bit : bits) {
        this->set (i, bit);
        i = i+1;
      }
    }

    T data () const { return this->_data; }

    void toggle (Index i) {
      this->set (i, ! this->get (i));
    }

    void set (Index i, bool value = true) {
      assert (i < sizeof(T) * 8);
      if (value)
        this->_data = this->_data | 1 << i;
      else
        this->_data = this->_data & ~(1 << i);
    }

    void reset (Index i) {
      this->set (i, false);
    }

    void reset () {
      this->_data = 0;
    }

    bool get (Index i) const {
      assert (i < sizeof(T) * 8);
      return this->_data & 1 << i;
    }

  private:
    T _data;
};

#endif

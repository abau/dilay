/* This file is part of Dilay
 * Copyright © 2015,2016 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_BITSET
#define DILAY_BITSET

#include <cmath>

template <typename T> class Bitset
{
public:
  Bitset ()
    : _data (0)
  {
  }

  Bitset (const Bitset& other)
    : _data (other._data)
  {
  }

  template <unsigned short N> bool get () const
  {
    static_assert (N < sizeof (T) * 8, "invalid bitset index");
    return this->_data & 1 << N;
  }

  template <unsigned short N> void set (bool value = true)
  {
    static_assert (N < sizeof (T) * 8, "invalid bitset index");

    if (value)
    {
      this->_data = this->_data | 1 << N;
    }
    else
    {
      this->_data = this->_data & ~(1 << N);
    }
  }

  template <unsigned short N> void toggle ()
  {
    this->set<N> (!this->get<N> ());
  }

  template <unsigned short N> void reset ()
  {
    this->set<N> (false);
  }

  void reset ()
  {
    this->_data = 0;
  }

  bool none () const
  {
    return this->_data == 0;
  }

  template <unsigned short N> bool all () const
  {
    static_assert (N < sizeof (T) * 8, "invalid bitset index");

    const T allBits = std::pow (2, N) - 1;
    return (this->_data & allBits) == allBits;
  }

  T value () const
  {
    return this->_data;
  }

private:
  T _data;
};

#endif

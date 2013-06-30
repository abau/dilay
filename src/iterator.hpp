#ifndef DILAY_ITERATOR
#define DILAY_ITERATOR

template <class T>
class Iterator {
  virtual bool isValid () const = 0;
  virtual void next    ()       = 0;
  virtual T    element () const = 0; 
};

template <class T>
class RefIterator {
  virtual bool isValid () const = 0;
  virtual void next    ()       = 0;
  virtual T&   element ()       = 0; 
};

template <class T>
class ConstRefIterator {
  virtual       bool isValid () const = 0;
  virtual       void next    ()       = 0;
  virtual const T&   element () const = 0; 
};

#endif

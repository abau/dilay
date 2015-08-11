/* This file is part of Dilay
 * Copyright © 2015 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_INTRUSIVE_LIST
#define DILAY_INTRUSIVE_LIST

#include <algorithm>
#include <functional>
#include <vector>
#include "maybe.hpp"

template <typename T>
class IntrusiveList {
  public:
    class Item {
      public:
        Item ()
          : _previous (nullptr)
        {}

        Item (const Item&)
          : _previous (nullptr)
        {}

        Item (Item&& o)
          : _previous (nullptr)
        {}

        Item& operator= (const Item&) {
          return *this;
        }

        Item& operator= (Item&&) {
          return *this;
        }

      protected:
        ~Item () {}

      private:
        friend class IntrusiveList <T>;

        T*        _previous;
        Maybe <T> _next;
    };

    IntrusiveList ()
      : _back        (nullptr)
      , _numElements (0)
    {}

    IntrusiveList (const IntrusiveList& o) 
      : IntrusiveList ()
    {
      this->operator= (o);
    }

    IntrusiveList (IntrusiveList&& o)
      : IntrusiveList ()
    {
      this->operator= (std::move (o));
    }

    ~IntrusiveList () {
      this->reset ();
    }

    IntrusiveList& operator= (const IntrusiveList& o) {
      if (this != &o) {
        this->reset ();

        o.forEachConstElement ([this] (const T& element) {
          this->emplaceBack (element);
        });
      }
      return *this;
    }

    IntrusiveList& operator= (IntrusiveList&& o) {
      if (this != &o) {
        this->reset ();

        this->_front       = std::move (o._front);
        this->_numElements = o._numElements;
        this->_back        = this->_front.get ();
        o._numElements     = 0;
        o._back            = nullptr;

        if (this->_back) {
          while (this->_back->_next) {
            this->_back = this->_back->_next.get ();
          }
        }
      }
      return *this;
    }

    unsigned int numElements () const {
      return this->_numElements;
    }

    bool isEmpty () const {
      return this->_numElements == 0;
    }

    T& front () {
      assert (this->_front);
      return *this->_front;
    }

    const T& front () const {
      assert (this->_front);
      return *this->_front;
    }

    T& back () {
      assert (this->_back);
      return *this->_back;
    }

    const T& back () const {
      assert (this->_back);
      return *this->_back;
    }

    template <typename ... Args>
    T& emplaceBack (Args&& ... args) {
      if (this->_back) {
        assert (this->_front);
        assert (this->_back->_next == false);

        this->_back->_next            = Maybe <T>::make (std::forward <Args> (args) ...);
        this->_back->_next->_previous = this->_back;
        this->_back                   = this->_back->_next.get ();
      }
      else {
        assert (this->_front == false);

        this->_front           = Maybe <T>::make (std::forward <Args> (args) ...);
        this->_back            = this->_front.get ();
        this->_back->_previous = nullptr;
      }
      this->_numElements++;
      return *this->_back;
    }

    void popBack () {
      assert (this->_front);
      assert (this->_back);
      assert (this->_numElements > 0);

      this->_back = this->_back->_previous;
      if (this->_back) {
        this->_back->_next.reset ();
      }
      else {
        assert (this->_numElements == 1);
        this->_front.reset ();
      }
      this->_numElements--;
    }

    void popFront () {
      assert (this->_front);
      assert (this->_back);
      assert (this->_numElements > 0);

      this->_front = std::move (this->_front->_next);
      if (this->_front) {
        this->_front->_previous = nullptr;
      }
      else {
        assert (this->_numElements == 1);
        this->_back = nullptr;
      }
      this->_numElements--;
    }

    void deleteElement (T& element) {
      assert (this->_front);
      assert (this->_back);
      assert (this->_numElements > 0);

      if (element._previous == nullptr) {
        assert (this->_front.get () == &element);
        this->popFront ();
      }
      else if (element._next == false) {
        assert (this->_back == &element);
        this->popBack ();
      }
      else {
        element._next->_previous = element._previous;
        element._previous->_next = std::move (element._next);
        this->_numElements--;
      }
    }

    void deleteElementIf (const std::function <bool (T&)>& f) {
      this->forEachElement ([this, &f] (T& e) {
        if (f (e)) {
          this->deleteElement (e);
        }
      });
    }

    void reset () {
      while (this->isEmpty () == false) {
        this->popBack ();
      }
    }

    void forEachElement (const std::function <void (T&)>& f) {
      T* ptr = this->_front.get ();
      while (ptr) {
        T* next = ptr->_next.get ();
        f (*ptr);
        ptr = next;
      }
    }

    void forEachConstElement (const std::function <void (const T&)>& f) const {
      const T* ptr = this->_front.get ();
      while (ptr) {
        f (*ptr);
        ptr = ptr->_next.get ();
      }
    }

  private:
    Maybe <T>    _front;
    T*           _back;
    unsigned int _numElements;
};

template <typename T>
class IntrusiveIndexedList {
  public:
    unsigned int numElements () const { return this->_list.numElements (); }
    bool         isEmpty     () const { return this->_list.isEmpty (); }
    T&           front       ()       { return this->_list.front (); }
    const T&     front       () const { return this->_list.front (); }
    T&           back        ()       { return this->_list.back  (); }
    const T&     back        () const { return this->_list.back  (); }

    template <typename ... Args>
    T& emplaceBack (const Args& ... args) {
      if (this->hasFreeIndices ()) {
        const unsigned int index = this->_freeIndices.back ();
        this->_freeIndices.pop_back ();
        this->_pointer.at (index) = &this->_list.emplaceBack (index, args ...);
        return *this->_pointer.at (index);
      }
      else {
        this->_pointer.push_back (&this->_list.emplaceBack (this->numElements (), args ...));
        return *this->_pointer.back ();
      }
    }

    void deleteElement (T& element) {
      assert (this->isFreeSLOW (element.index ()) == false);

      this->_freeIndices.push_back (element.index ());
      this->_pointer.at (element.index ()) = nullptr;
      this->_list.deleteElement (element);
    }

    void reset () {
      this->_list       .reset ();
      this->_freeIndices.clear ();
      this->_pointer    .clear ();
    }

    void forEachElement (const std::function <void (T&)>& f) {
      this->_list.forEachElement (f);
    }

    void forEachConstElement (const std::function <void (const T&)>& f) const {
      this->_list.forEachConstElement (f);
    }

          T* get (unsigned int index)       { return this->_pointer.at (index); }
    const T* get (unsigned int index) const { return this->_pointer.at (index); }

    const std::vector <unsigned int>& freeIndices () const { return this->_freeIndices; }

    bool hasFreeIndices () const {
      return this->_freeIndices.empty () == false;
    }

    bool isFreeSLOW (unsigned int index) const {
      return std::find (this->_freeIndices.begin (), this->_freeIndices.end (), index)
          != this->_freeIndices.end ();
    }

  private:
    IntrusiveList <T>          _list;
    std::vector <unsigned int> _freeIndices;
    std::vector <T*>           _pointer;
};

#endif

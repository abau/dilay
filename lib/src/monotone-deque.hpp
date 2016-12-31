/* This file is part of Dilay
 * Copyright © 2015,2016 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_MONOTONE_DEQUE
#define DILAY_MONOTONE_DEQUE

#include <deque>
#include <functional>
#include <memory>
#include <unordered_set>
#include "util.hpp"

template <typename T>
class MonotoneDeque {
  public:
    unsigned int numElements () const {
      return this->elements.size () - this->freeIndices.size ();
    }

    bool isEmpty () const {
      return this->numElements () == 0;
    }

    bool hasFreeIndices () const {
      return this->freeIndices.empty () == false;
    }

    bool isFree (unsigned int index) const {
      return this->freeIndices.find (index) != this->freeIndices.end ();
    }

    unsigned int findSomeNonFreeIndex () const {
      assert (this->numElements () > 0);

      for (unsigned int i = 0; i < this->elements.size (); i++) {
        if (this->isFree (i) == false) {
          return i;
        }
      }
      DILAY_IMPOSSIBLE
    }

    unsigned int nextFreeIndex () const {
      if (this->hasFreeIndices ()) {
        return *this->freeIndices.begin ();
      }
      else {
        return this->elements.size ();
      }
    }

    T& elementRef (unsigned int index) {
      assert (index < this->elements.size ());
      assert (this->isFree (index) == false);

      return this->elements [index];
    }

    const T& elementRef (unsigned int index) const {
      assert (index < this->elements.size ());
      assert (this->isFree (index) == false);

      return this->elements [index];
    }

    T* element (unsigned int index) {
      if (index < this->elements.size () && this->isFree (index) == false) {
        return &this->elements [index];
      }
      else {
        return nullptr;
      }
    }

    const T* element (unsigned int index) const {
      if (index < this->elements.size () && this->isFree (index) == false) {
        return &this->elements [index];
      }
      else {
        return nullptr;
      }
    }

    T& insertElement (unsigned int index, T&& element) {
      assert (index == this->nextFreeIndex ());

      if (index == this->elements.size ()) {
        this->elements.push_back (std::move (element));
        return this->elements.back ();
      }
      else if (index == *this->freeIndices.begin ()) {
        this->freeIndices.erase (this->freeIndices.begin ());

        this->elements [index] = std::move (element);
        return this->elements [index];
      }
      else {
        DILAY_IMPOSSIBLE
      }
    }

    template <typename ... Args>
    T& emplaceElement (Args&& ... args) {
      const unsigned int index = this->nextFreeIndex ();

      return this->insertElement (index, T (index, std::forward <Args> (args) ...));
    }

    void deleteElement (unsigned int index) {
      this->freeIndices.insert (index);
    }

    void deleteElement (T& element) {
      this->deleteElement (element.index ());
    }

    void reset () {
      this->elements.clear ();
      this->freeIndices.clear ();
    }

    void forEachElement (const std::function <void (T&)>& f) {
      for (unsigned int i = 0; i < this->elements.size (); i++) {
        if (this->isFree (i) == false) {
          f (this->elements [i]);
        }
      }
    }

    void forEachConstElement (const std::function <void (const T&)>& f) const {
      for (unsigned int i = 0; i < this->elements.size (); i++) {
        if (this->isFree (i) == false) {
          f (this->elements [i]);
        }
      }
    }

    void forEachFreeIndex (const std::function <void (unsigned int)>& f) const {
      for (unsigned int index : this->freeIndices) {
        f (index);
      }
    }

  private:
    std::deque <T> elements;
    std::unordered_set <unsigned int> freeIndices;
};

template <typename T>
class MonotonePtrDeque {
  public:
    unsigned int numElements () const {
      return this->vector.numElements ();
    }

    bool isEmpty () const {
      return this->vector.isEmpty ();
    }

    bool hasFreeIndices () const {
      return this->vector.hasFreeIndices ();
    }

    bool isFree (unsigned int index) const {
      return this->vector.isFree (index);
    }

    unsigned int findSomeNonFreeIndex () const {
      return this->vector.findSomeNonFreeIndex ();
    }

    T& elementRef (unsigned int index) {
      return *this->vector.elementRef (index);
    }

    const T& elementRef (unsigned int index) const {
      return *this->vector.elementRef (index);
    }

    T* element (unsigned int index) {
      auto* ptr = this->vector.element (index);

      if (ptr) {
        assert (ptr->get ());
        return ptr->get ();
      }
      else {
        return nullptr;
      }
    }

    const T* element (unsigned int index) const {
      const auto* ptr = this->vector.element (index);

      if (ptr) {
        assert (ptr->get ());
        return ptr->get ();
      }
      else {
        return nullptr;
      }
    }

    template <typename ... Args>
    T& emplaceElement (Args&& ... args) {
      const unsigned int  index = this->vector.nextFreeIndex ();
      std::unique_ptr <T> ptr   = std::make_unique <T> (index, std::forward <Args> (args) ...);

      return *this->vector.insertElement (index, std::move (ptr));
    }

    void deleteElement (unsigned int index) {
      this->vector.deleteElement (index);
    }

    void deleteElement (T& element) {
      this->deleteElement (element.index ());
    }

    void reset () {
      this->vector.reset ();
    }

    void forEachElement (const std::function <void (T&)>& f) {
      this->vector.forEachElement ([&f] (auto& element) {
        assert (element);
        f (*element);
      });
    }

    void forEachConstElement (const std::function <void (const T&)>& f) const {
      this->vector.forEachConstElement ([&f] (const auto& element) {
        assert (element);
        f (*element);
      });
    }

    void forEachFreeIndex (const std::function <void (unsigned int)>& f) const {
      this->vector.forEachFreeIndex (f);
    }

  private:
    MonotoneDeque <std::unique_ptr <T>> vector;
};

#endif

/* This file is part of Dilay
 * Copyright © 2015 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */

#ifndef DILAY_TREE
#define DILAY_TREE

#include <list>
#include <memory>
#include "util.hpp"

template <typename T>
class Tree {
  public:
    template <typename ... Args>
    Tree (Tree* p, Args&& ... args)
      : _parent (p)
      , _data   (std::forward <Args> (args) ... )
    {}

    Tree (const Tree& o)
      : _parent   (nullptr)
      , _data     (o._data)
      , _children (o._children)
    {
      this->forEachChild ([this] (Tree& t) {
        t.parent (this);
      });
    }

    Tree*       parent ()       { return this->_parent; }
    const Tree* parent () const { return this->_parent; }
    T&          data   ()       { return this->_data; }
    const T&    data   () const { return this->_data; }

    void parent (Tree* p)    { this->_parent = p; }
    void data   (const T& d) { this->_data   = d; }

    template <typename ... Args>
    Tree& emplaceChild (Args&& ... args) {
      this->_children.emplace_back (this, std::forward <Args> (args) ... );
      return this->_children.back ();
    }

    void deleteChild (Tree& child) {
      for (auto it = this->_children.begin (); it != this->_children.end (); ++it) {
        if (&child == &*it) {
          this->_children.erase (it);
          return;
        }
      }
      DILAY_IMPOSSIBLE
    }

    void forEachChild (const std::function <void (Tree&)>& f) {
      for (Tree& c : this->_children) {
        f (c);
      }
    }

    void forEachConstChild (const std::function <void (const Tree&)>& f) const {
      for (const Tree& c : this->_children) {
        f (c);
      }
    }

    void forEachTree (const std::function <void (Tree&)>& f) {
      f (*this);
      this->forEachChild ([&f] (Tree& c) { c.forEachTree (f); });
    }

    void forEachConstTree (const std::function <void (const Tree&)>& f) const {
      f (*this);
      this->forEachConstChild ([&f] (const Tree& c) { c.forEachConstTree (f); });
    }

    unsigned int numElements () const {
      unsigned int n = 0;
      this->forEachConstTree ([&n] (const Tree <T>&) { n++; });
      return n;
    }

  private:
    Tree*            _parent;
    T                _data;
    std::list <Tree> _children;
};

#endif

/* This file is part of Dilay
 * Copyright © 2015 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_TREE
#define DILAY_TREE

#include <unordered_map>
#include "intrusive-list.hpp"
#include "maybe.hpp"

template <typename T> 
class TreeNode;

template <typename T> 
using TreeNodePtrMap = std::unordered_map <const TreeNode <T>*, TreeNode <T>*>;

template <typename T>
class TreeNode : public IntrusiveList <TreeNode <T>>::Item {
  public:
    TreeNode (const T& d)
      : _data (d)
    {}

    TreeNode (T&& d)
      : _data (std::move (d))
    {}

    explicit TreeNode (const TreeNode& o, TreeNodePtrMap <T>& ptrMap)
      : _data (o._data)
    {
      ptrMap.emplace (&o, this);

      o._children.forEachConstElement ([this, &ptrMap] (const TreeNode& c) {
        this->_children.emplaceBack (c, ptrMap);
      });
    }

    template <typename ... Args>
    static TreeNode make (Args&& ... args) {
      return TreeNode (T (std::forward <Args> (args) ... ));
    }

    T&       data ()           { return this->_data; }
    const T& data () const     { return this->_data; }
    void     data (const T& d) { this->_data = d;    }

    template <typename ... Args>
    TreeNode& emplaceChild (Args&& ... args) {
      this->_children.emplaceBack (make (std::forward <Args> (args) ... ));
      return this->_children.back ();
    }

    void deleteChild (TreeNode& child) {
      this->_children.deleteElement (child);
    }

    void forEachChild (const std::function <void (TreeNode&)>& f) {
      this->_children.forEachElement (f);
    }

    void forEachConstChild (const std::function <void (const TreeNode&)>& f) const {
      this->_children.forEachConstElement (f);
    }

    void forEachNode (const std::function <void (TreeNode&)>& f) {
      f (*this);

      this->forEachChild ([&f] (TreeNode& c) {
        c.forEachNode (f);
      });
    }

    void forEachConstNode (const std::function <void (const TreeNode&)>& f) const {
      f (*this);

      this->forEachConstChild ([&f] (const TreeNode& c) {
        c.forEachConstNode (f);
      });
    }

    TreeNode& lastChild () {
      assert (this->numChildren () > 0);
      return this->_children.back ();
    }

    const TreeNode& lastChild () const {
      assert (this->numChildren () > 0);
      return this->_children.back ();
    }

    unsigned int numChildren () const {
      return this->_children.numElements ();
    }

    unsigned int numNodes () const {
      unsigned int n = 0;
      this->forEachConstNode ([&n] (const TreeNode <T>&) { n++; });
      return n;
    }

    void deleteChildIf (const std::function <bool (const TreeNode&)>& f) {
      this->_children.deleteElementIf (f);
    }

  private:
    T                        _data;
    IntrusiveList <TreeNode> _children;
};

template <typename T>
class Tree {
  public:
    Tree () {}

    Tree (const Tree& o) {
      this->operator= (o);
    }

    Tree& operator= (const Tree& o) {
      this->reset ();

      if (o.hasRoot ()) {
        TreeNodePtrMap <T> ptrMap;
        ptrMap.emplace (nullptr, nullptr);

        this->_root = Maybe <TreeNode <T>>::make (o.root (), ptrMap);
        this->_root->forEachNode ([&ptrMap] (TreeNode <T>& node) {
          node.data ().updatePointers (ptrMap);
        });
      }
      return *this;
    }

    bool hasRoot () const {
      return bool (this->_root);
    }

    TreeNode <T>& root () {
      assert (this->hasRoot ());
      return *this->_root;
    }

    const TreeNode <T>& root () const {
      assert (this->hasRoot ());
      return *this->_root;
    }

    template <typename ... Args>
    TreeNode <T>& emplaceRoot (Args&& ... args) {
      this->_root = Maybe <TreeNode <T>>::make 
                          (TreeNode <T> ::make (std::forward <Args> (args) ...));
      return this->root ();
    }

    void reset () {
      this->_root.reset ();
    }

  private:
    Maybe <TreeNode <T>> _root;
};

#endif

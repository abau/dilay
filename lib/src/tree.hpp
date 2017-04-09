/* This file is part of Dilay
 * Copyright © 2015,2016 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_TREE
#define DILAY_TREE

#include <list>
#include "maybe.hpp"
#include "util.hpp"

template <typename T> class TreeNode
{
public:
  TreeNode (const T& d, TreeNode* p = nullptr)
    : _data (d)
    , _parent (p)
  {
  }

  TreeNode (T&& d, TreeNode* p = nullptr)
    : _data (std::move (d))
    , _parent (p)
  {
  }

  TreeNode (const TreeNode& o)
    : _data (o._data)
    , _parent (nullptr)
    , _children (o._children)
  {
    this->forEachChild ([this](TreeNode& c) { c._parent = this; });
  }

  template <typename... Args> static TreeNode make (Args&&... args)
  {
    return TreeNode (T (std::forward<Args> (args)...));
  }

  T& data ()
  {
    return this->_data;
  }

  const T& data () const
  {
    return this->_data;
  }

  void data (const T& d)
  {
    this->_data = d;
  }

  TreeNode* parent () const
  {
    return this->_parent;
  }

  template <typename... Args> TreeNode& emplaceChild (Args&&... args)
  {
    this->_children.emplace_back (make (std::forward<Args> (args)...));
    this->_children.back ()._parent = this;

    return this->_children.back ();
  }

  TreeNode& addChild (const TreeNode& node)
  {
    this->_children.emplace_back (node);
    this->_children.back ()._parent = this;

    return this->_children.back ();
  }

  void deleteChild (TreeNode& child)
  {
    for (auto it = this->_children.begin (); it != this->_children.end (); ++it)
    {
      if (&*it == &child)
      {
        this->_children.erase (it);
        return;
      }
    }
    DILAY_IMPOSSIBLE
  }

  void forEachChild (const std::function<void(TreeNode&)>& f)
  {
    for (TreeNode& c : this->_children)
    {
      f (c);
    }
  }

  void forEachConstChild (const std::function<void(const TreeNode&)>& f) const
  {
    for (const TreeNode& c : this->_children)
    {
      f (c);
    }
  }

  void forEachNode (const std::function<void(TreeNode&)>& f)
  {
    f (*this);

    this->forEachChild ([&f](TreeNode& c) { c.forEachNode (f); });
  }

  void forEachConstNode (const std::function<void(const TreeNode&)>& f) const
  {
    f (*this);

    this->forEachConstChild ([&f](const TreeNode& c) { c.forEachConstNode (f); });
  }

  TreeNode& lastChild ()
  {
    assert (this->numChildren () > 0);
    return this->_children.back ();
  }

  const TreeNode& lastChild () const
  {
    assert (this->numChildren () > 0);
    return this->_children.back ();
  }

  unsigned int numChildren () const
  {
    return this->_children.size ();
  }

  unsigned int numNodes () const
  {
    unsigned int n = 0;
    this->forEachConstNode ([&n](const TreeNode<T>&) { n++; });
    return n;
  }

  void deleteChildIf (const std::function<bool(const TreeNode&)>& f)
  {
    for (auto it = this->_children.begin (); it != this->_children.end ();)
    {
      if (f (*it))
      {
        it = this->_children.erase (it);
      }
      else
      {
        ++it;
      }
    }
  }

private:
  T                   _data;
  TreeNode*           _parent;
  std::list<TreeNode> _children;
};

template <typename T> class Tree
{
public:
  bool hasRoot () const
  {
    return bool(this->_root);
  }

  TreeNode<T>& root ()
  {
    assert (this->hasRoot ());
    return *this->_root;
  }

  const TreeNode<T>& root () const
  {
    assert (this->hasRoot ());
    return *this->_root;
  }

  template <typename... Args> TreeNode<T>& emplaceRoot (Args&&... args)
  {
    this->_root = Maybe<TreeNode<T>>::make (TreeNode<T>::make (std::forward<Args> (args)...));
    return this->root ();
  }

  void reset ()
  {
    this->_root.reset ();
  }

  void rebalance (TreeNode<T>& node)
  {
    const std::function<void(const TreeNode<T>&, TreeNode<T>&)> go = [&go](const auto& child,
                                                                           auto& rebalancedChild) {
      if (child.parent ())
      {
        const auto& parent = *child.parent ();
        auto&       rebalancedParent = rebalancedChild.emplaceChild (parent.data ());

        parent.forEachConstChild ([&rebalancedParent, &child](const auto& c) {
          if (&c != &child)
          {
            rebalancedParent.addChild (c);
          }
        });
        go (parent, rebalancedParent);
      }
    };

    TreeNode<T>& newRoot (node);
    go (node, newRoot);
    this->_root = std::move (newRoot);
  }

private:
  Maybe<TreeNode<T>> _root;
};

#endif

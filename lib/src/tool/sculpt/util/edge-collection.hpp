/* This file is part of Dilay
 * Copyright © 2015-2018 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_TOOL_SCULPT_EDGE_COLLECTION
#define DILAY_TOOL_SCULPT_EDGE_COLLECTION

#include <unordered_map>
#include <unordered_set>
#include "hash.hpp"

class ToolSculptEdgeMap
{
public:
  typedef std::unordered_map<std::pair<unsigned int, unsigned int>, unsigned int> Map;

  void         insert (unsigned int, unsigned int, unsigned int);
  unsigned int find (unsigned int, unsigned int) const;
  bool         contains (unsigned int, unsigned int) const;
  bool         isEmpty () const;
  void         reset ();

private:
  Map map;
};

class ToolSculptEdgeSet
{
public:
  typedef std::unordered_set<std::pair<unsigned int, unsigned int>> Set;

  void insert (unsigned int, unsigned int);
  bool contains (unsigned int, unsigned int) const;
  bool isEmpty () const;
  void reset ();

  Set::const_iterator begin () const { return this->set.begin (); }
  Set::const_iterator end () const { return this->set.end (); }

private:
  Set set;
};

#endif

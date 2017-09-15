/* This file is part of Dilay
 * Copyright © 2015-2017 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <glm/glm.hpp>
#include "tool/sculpt/util/edge-collection.hpp"
#include "util.hpp"

namespace
{
  ui_pair makeUiKey (unsigned int i1, unsigned int i2)
  {
    assert (i1 != i2);
    return ui_pair (glm::min (i1, i2), glm::max (i1, i2));
  }
}

void ToolSculptEdgeMap::insert (unsigned int i1, unsigned int i2, unsigned int value)
{
  assert (this->contains (i1, i2) == false);
  this->map.emplace (makeUiKey (i1, i2), value);
}

unsigned int ToolSculptEdgeMap::find (unsigned int i1, unsigned int i2) const
{
  const auto it = this->map.find (makeUiKey (i1, i2));

  return it == this->map.end () ? Util::invalidIndex () : it->second;
}

bool ToolSculptEdgeMap::contains (unsigned int i1, unsigned int i2) const
{
  return this->find (i1, i2) != Util::invalidIndex ();
}

bool ToolSculptEdgeMap::isEmpty () const { return this->map.empty (); }

void ToolSculptEdgeMap::reset () { this->map.clear (); }

void ToolSculptEdgeSet::insert (unsigned int i1, unsigned int i2)
{
  this->set.emplace (makeUiKey (i1, i2));
}

bool ToolSculptEdgeSet::contains (unsigned int i1, unsigned int i2) const
{
  return this->set.find (makeUiKey (i1, i2)) != this->set.end ();
}

bool ToolSculptEdgeSet::isEmpty () const { return this->set.empty (); }

void ToolSculptEdgeSet::reset () { this->set.clear (); }

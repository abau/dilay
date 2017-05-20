/* This file is part of Dilay
 * Copyright © 2015-2017 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include "dynamic/faces.hpp"

void DynamicFaces::insert (unsigned int i) { this->_uncommitted.insert (i); }

void DynamicFaces::insert (const DynamicFaces::Container& v)
{
  this->_uncommitted.insert (v.begin (), v.end ());
}

void DynamicFaces::reset ()
{
  this->_indices.clear ();
  this->_uncommitted.clear ();
}

void DynamicFaces::commit ()
{
  if (this->_indices.empty ())
  {
    this->_indices.swap (this->_uncommitted);
  }
  else
  {
    this->_indices.insert (this->_uncommitted.begin (), this->_uncommitted.end ());
    this->_uncommitted.clear ();
  }
}

bool DynamicFaces::contains (unsigned int i) const
{
  return this->_indices.find (i) != this->_indices.end ();
}

bool DynamicFaces::isEmpty () const
{
  return this->_indices.empty () && this->_uncommitted.empty ();
}

bool DynamicFaces::hasUncomitted () const { return this->_uncommitted.empty () == false; }

void DynamicFaces::filter (const std::function<bool(unsigned int)>& f)
{
  for (auto it = this->_indices.begin (); it != this->_indices.end ();)
  {
    if (f (*it) == false)
    {
      it = this->_indices.erase (it);
    }
    else
    {
      ++it;
    }
  }
  for (auto it = this->_uncommitted.begin (); it != this->_uncommitted.end ();)
  {
    if (f (*it) == false)
    {
      it = this->_uncommitted.erase (it);
    }
    else
    {
      ++it;
    }
  }
}

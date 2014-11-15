#ifndef DILAY_SELECTION
#define DILAY_SELECTION

#include <functional>
#include <unordered_set>
#include "macro.hpp"

class Id;

class Selection {
  public:
    DECLARE_BIG6 (Selection)

    void          selectMajor   (const Id&);
    void          selectMinor   (const Id&, unsigned int);

    void          unselectMajor (const Id&);
    void          unselectMinor (const Id&, unsigned int);

    bool          hasMajor      (const Id&) const;
    bool          hasMinor      (const Id&, unsigned int) const;

    void          toggleMajor   (const Id&);
    void          toggleMinor   (const Id&, unsigned int);

    void          forEachMajor  (const std::function <void (const Id&)>&) const;
    void          forEachMinor  (const std::function <void (const Id&, unsigned int)>&) const;

    void          reset         ();
    void          resetMajors   ();
    void          resetMinors   ();

    unsigned int  numMajors     () const;
    unsigned int  numMinors     () const;

  private:
    IMPLEMENTATION
};

#endif

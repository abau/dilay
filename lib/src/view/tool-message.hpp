#ifndef DILAY_VIEW_TOOL_MESSAGE
#define DILAY_VIEW_TOOL_MESSAGE

#include <initializer_list>
#include "macro.hpp"

class QString;

class ViewToolMessage {
  public:
    DECLARE_BIG6 (ViewToolMessage, const QString&)

           QString   message () const;
    static QString   message (const std::initializer_list <ViewToolMessage>&);

    ViewToolMessage& left   ();
    ViewToolMessage& middle ();
    ViewToolMessage& right  ();

    ViewToolMessage& ctrl   ();
    ViewToolMessage& shift  ();
    ViewToolMessage& alt    ();

  private:
    IMPLEMENTATION
};

#endif

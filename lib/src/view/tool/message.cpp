#include <QObject>
#include <QString>
#include "view/tool/message.hpp"

struct ViewToolMessage::Impl {
  ViewToolMessage* self;
  const QString    action;
        QString    mouseButton;
        QString    modifier;

  Impl (ViewToolMessage* s, const QString& a)
    : self        (s)
    , action      (a)
    , mouseButton ("")
    , modifier    ("")
  {}

  QString message () const {
    assert (this->mouseButton.isEmpty () == false);
    assert (this->action     .isEmpty () == false);

    QString input = this->modifier.isEmpty () ? this->mouseButton
                                              : this->mouseButton + " + " + this->modifier;
    return "[" + input + "] " + this->action;
  }

  static QString message (const std::initializer_list <ViewToolMessage>& messages) {
          QString      result;
    const unsigned int numSpaces = 5;

    for (const ViewToolMessage& m : messages) {
      result.append (m.message ()).append (QString (" ").repeated (numSpaces));;
    }
    result.remove (result.size () - numSpaces, numSpaces);
    return result;
  }

  ViewToolMessage& left () { 
    this->mouseButton = QObject::tr ("Left"); 
    return *this->self;
  }

  ViewToolMessage& middle () { 
    this->mouseButton = QObject::tr ("Middle");
    return *this->self;
  }

  ViewToolMessage& right () { 
    this->mouseButton = QObject::tr ("Right");
    return *this->self;
  }

  ViewToolMessage& ctrl () { 
    this->modifier = QObject::tr ("Ctrl");
    return *this->self;
  }

  ViewToolMessage& shift () { 
    this->modifier = QObject::tr ("Shift");
    return *this->self;
  }

  ViewToolMessage& alt () { 
    this->modifier = QObject::tr ("Alt");
    return *this->self;
  }
};

DELEGATE1_BIG4COPY_SELF (ViewToolMessage, const QString&)

DELEGATE_CONST   (QString         , ViewToolMessage, message)
DELEGATE1_STATIC (QString         , ViewToolMessage, message, const std::initializer_list<ViewToolMessage>&)
DELEGATE         (ViewToolMessage&, ViewToolMessage, left)
DELEGATE         (ViewToolMessage&, ViewToolMessage, middle)
DELEGATE         (ViewToolMessage&, ViewToolMessage, right)
DELEGATE         (ViewToolMessage&, ViewToolMessage, ctrl)
DELEGATE         (ViewToolMessage&, ViewToolMessage, shift)
DELEGATE         (ViewToolMessage&, ViewToolMessage, alt)

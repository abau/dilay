#ifndef LINKED_LIST
#define LINKED_LIST

// LinkedElement //////////////////////////////////////////
template <class T>
class LinkedElement {
  public:
    LinkedElement (const T&, LinkedElement <T>*, LinkedElement <T>*);

    T&                       data ()        { return this->_data; }
    const T&                 data () const  { return this->_data; }
    LinkedElement <T>*       prev ()        { return this->_prev; }
    const LinkedElement <T>* prev () const  { return this->_prev; }
    LinkedElement <T>*       next ()        { return this->_next; }
    const LinkedElement <T>* next () const  { return this->_next; }

    void                     setPrev (LinkedElement <T>*);
    void                     setNext (LinkedElement <T>*);
    bool                     hasPrev () const;
    bool                     hasNext () const;

  private:
    LinkedElement (const LinkedElement <T>&) {}

    T                  _data;
    LinkedElement <T>* _prev;
    LinkedElement <T>* _next;

};

template <class T>
LinkedElement <T> :: LinkedElement (const T& t, LinkedElement <T>* p, LinkedElement <T>* n) 
  : _data (t) {
      this->_prev = p;
      this->_next = n;
}

template <class T>
void LinkedElement <T> :: setPrev (LinkedElement <T>* p) { this->_prev = p; }

template <class T>
void LinkedElement <T> :: setNext (LinkedElement <T>* n) { this->_next = n; }

template <class T>
bool LinkedElement <T> :: hasPrev () const { return this->_prev != 0; }

template <class T>
bool LinkedElement <T> :: hasNext () const { return this->_next != 0; }

// LinkedIterator /////////////////////////////////////////
template <class T>
class LinkedIterator {
  public:
    LinkedIterator (LinkedElement <T>*);

    LinkedElement <T>* linkedElement () { return this->element; }
    T&                 data ()          { return this->element->data (); }
    LinkedElement <T>* prev ();
    LinkedElement <T>* next ();
    bool               hasElement () const;

  private:
    LinkedElement <T>* element;
};

template <class T>
LinkedIterator <T> :: LinkedIterator (LinkedElement <T>* start) {
  this->element = start;
}

template <class T>
LinkedElement <T>* LinkedIterator <T> :: prev () { 
  this->element = this->element->prev (); 
  return this->linkedElement ();
}

template <class T>
LinkedElement <T>* LinkedIterator <T> :: next () { 
  this->element = this->element->next (); 
  return this->linkedElement ();
}

template <class T>
bool LinkedIterator <T> :: hasElement () const { return this->element != 0; }

// LinkedConstIterator ////////////////////////////////////
template <class T>
class LinkedConstIterator {
  public:
    LinkedConstIterator (const LinkedElement <T>*);

    const LinkedElement <T>* linkedElement () const { return this->element; };
    const T&                 data ()          const { return this->element->data (); }
    const LinkedElement <T>* prev ();
    const LinkedElement <T>* next ();
    bool                     hasElement () const;

  private:
    const LinkedElement <T>* element;
};

template <class T>
LinkedConstIterator <T> :: LinkedConstIterator (const LinkedElement <T>* start) {
  this->element = start;
}

template <class T>
const LinkedElement <T>* LinkedConstIterator <T> :: prev () { 
  this->element = this->element->prev (); 
  return this->linkedElement ();
}

template <class T>
const LinkedElement <T>* LinkedConstIterator <T> :: next () { 
  this->element = this->element->next (); 
  return this->linkedElement ();
}

template <class T>
bool LinkedConstIterator <T> :: hasElement () const { return this->element != 0; }

// LinkedList /////////////////////////////////////////////
template <class T>
class LinkedList {
  public:
    typedef LinkedElement <T>      Element;
    typedef LinkedIterator<T>      Iterator;
    typedef LinkedConstIterator<T> ConstIterator;

    LinkedList  ();
    LinkedList  (const LinkedList <T>&);
    ~LinkedList ()                       { this->eraseAll (); }

    unsigned int  numElements   () const { return this->_numElements; }
    Element*      insertFront   (const T&);
    Element*      insertBack    (const T&);
    void          erase         (LinkedElement <T>*);
    void          eraseAll      ();
    Iterator      frontIterator ();
    ConstIterator frontIterator () const;
    Iterator      backIterator  (); 
    ConstIterator backIterator  () const; 

  private:
    unsigned int       _numElements;
    LinkedElement <T>* start;
    LinkedElement <T>* end;
};

template <class T>
LinkedList <T> :: LinkedList () {
  this->_numElements = 0;
  this->start        = 0;
  this->end          = 0;
}

template <class T>
LinkedList <T> :: LinkedList (const LinkedList <T>& source) {
  this->_numElements = 0;
  this->start        = 0;
  this->end          = 0;

  ConstIterator it = source.frontIterator ();
  while (it.hasElement ()) {
    this->insertBack (it.data ());
    it.next ();
  }
}

template <class T>
LinkedElement <T>* LinkedList <T> :: insertFront (const T& e) { 
  LinkedElement <T>* newElement = new LinkedElement <T> (e, 0, this->start);

  if (newElement->hasNext ())
    newElement->next ()->setPrev (newElement);

  this->start = newElement;

  if (this->end == 0)
    this->end = newElement;

  this->_numElements++;
  return newElement;
}

template <class T>
LinkedElement <T>* LinkedList <T> :: insertBack (const T& e) { 
  LinkedElement <T>* newElement = new LinkedElement <T> (e, this->end, 0);

  if (newElement->hasPrev ())
    newElement->prev ()->setNext (newElement);

  this->end = newElement;

  if (this->start == 0)
    this->start = newElement;

  this->_numElements++;
  return newElement;
}

template <class T>
void LinkedList <T> :: erase (LinkedElement <T>* e) { 
  if (e == this->start) 
    this->start = e->next ();
 
  if (e == this->end) 
    this->end = e->prev ();
 
  if (e->hasPrev ())
    e->prev ()->setNext (e->next ());

  if (e->hasNext ()) 
    e->next ()->setPrev (e->prev ());

  delete (e);
  this->_numElements--;
}

template <class T>
void LinkedList <T> :: eraseAll () { 
  LinkedIterator <T> it = this->frontIterator ();
  while (it.hasElement ()) {
    LinkedElement <T>* e = it.linkedElement ();
    it.next ();
    this->erase (e);
  }
}

template <class T>
LinkedIterator <T> LinkedList <T> :: frontIterator () {
  return LinkedIterator <T> (this->start);
}

template <class T>
LinkedConstIterator <T> LinkedList <T> :: frontIterator () const {
  return LinkedConstIterator <T> (this->start);
}

template <class T>
LinkedIterator <T> LinkedList <T> :: backIterator () {
  return LinkedIterator <T> (this->end);
}

template <class T>
LinkedConstIterator <T> LinkedList <T> :: backIterator () const {
  return LinkedIterator <T> (this->end);
}

#endif

#include <sstream>

#ifndef DEPTH
#define DEPTH

class Depth {
  public:
    Depth () : _value (0) {}
    Depth (int v) : _value (v) {}

    static Depth null      ()       { return Depth (0); }

    int          value     () const { return this->_value; }
    void         increase  ()       { this->_value ++; }
    void         decrease  ()       { this->_value --; }
    Depth        increased ()       { return Depth (this->_value + 1); }
    Depth        decreased ()       { return Depth (this->_value - 1); }

    bool operator<  (const Depth& d) { return this->_value <  d.value (); }
    bool operator<= (const Depth& d) { return this->_value <= d.value (); }
    bool operator== (const Depth& d) { return this->_value == d.value (); }
    bool operator!= (const Depth& d) { return this->_value != d.value (); }
    bool operator>= (const Depth& d) { return this->_value >= d.value (); }
    bool operator>  (const Depth& d) { return this->_value >  d.value (); }

  private:
    int _value;
};

std::ostream& operator<<(std::ostream&, const Depth&);
#endif

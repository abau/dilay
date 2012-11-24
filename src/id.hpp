#ifndef IDD
#define IDD

typedef unsigned long IdType;

class Id {
  private:
    static IdType nextId;

    const  IdType _id;

  public:
    Id ();

    IdType get () const { return this->_id; }
};
#endif

#ifndef IDD
#define IDD

typedef unsigned long IdType;

class Id {
  public:
    Id ();

    IdType get () const { return this->_id; }

  private:
    static IdType nextId;
    const  IdType _id;
};
#endif

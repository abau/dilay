#ifndef IDD
#define IDD

typedef unsigned long IdType;

class Id {
  public:     Id        ();
              Id        (const Id&);
    const Id& operator= (const Id&);

    IdType    get       () const { return this->_id; }

  private:
    const  IdType _id;

    static IdType nextId ();
};
#endif

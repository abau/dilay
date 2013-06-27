#ifndef DILAY_IDD
#define DILAY_IDD

typedef unsigned long IdType;

class Id {
  public:     Id        (IdType = Id::invalidId ());
              Id        (const Id&);
    const Id& operator= (const Id&);

           IdType get       () const { return this->_id; }
    static IdType invalidId ()       { return 0; }

  private:
    const  IdType _id;

    static IdType nextId ();
};
#endif

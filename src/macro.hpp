#ifndef DILAY_MACRO
#define DILAY_MACRO

#include <cassert>

// Delegators

#define DELEGATE_CONSTRUCTOR(from) \
  from :: from () { this->impl = new Impl (); }

#define DELEGATE_COPY_CONSTRUCTOR(from) \
  from :: from (const from & a1) { this->impl = new Impl (*a1.impl); }

#define DELEGATE_MOVE_CONSTRUCTOR(from) \
  from :: from (from && a1) { \
    this->impl = a1.impl; \
    a1.impl    = nullptr; }

#define DELEGATE_ASSIGNMENT_OP(from) \
  const from & from :: operator= (const from & source) { \
    if (this == &source) return *this; \
    from tmp (source); \
    Impl * tmpImpl = tmp.impl; \
    tmp.impl       = this->impl; \
    this->impl     = tmpImpl; \
    return *this; }

#define DELEGATE_MOVE_ASSIGNMENT_OP(from) \
  const from & from :: operator= (from && source) { \
    if (this == &source) return *this; \
    Impl * tmpImpl = source.impl; \
    source.impl    = this->impl; \
    this->impl     = tmpImpl; \
    return *this; }

#define DELEGATE1_CONSTRUCTOR(from,t1) \
  from :: from (t1 a1) { this->impl = new Impl (a1); }

#define DELEGATE2_CONSTRUCTOR(from,t1,t2) \
  from :: from (t1 a1,t2 a2) { this->impl = new Impl (a1,a2); }

#define DELEGATE3_CONSTRUCTOR(from,t1,t2,t3) \
  from :: from (t1 a1,t2 a2,t3 a3) { this->impl = new Impl (a1,a2,a3); }

#define DELEGATE4_CONSTRUCTOR(from,t1,t2,t3,t4) \
  from :: from (t1 a1,t2 a2,t3 a3,t4 a4) { this->impl = new Impl (a1,a2,a3,a4); }

#define DELEGATE5_CONSTRUCTOR(from,t1,t2,t3,t4,t5) \
  from :: from (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5) { this->impl = new Impl (a1,a2,a3,a4,a5); }

#define DELEGATE_DESTRUCTOR(from) from :: ~ from () { delete this->impl; }

#define DELEGATE_BIG6(from) \
  DELEGATE_CONSTRUCTOR(from) \
  DELEGATE_DESTRUCTOR(from) \
  DELEGATE_COPY_CONSTRUCTOR(from) \
  DELEGATE_MOVE_CONSTRUCTOR(from) \
  DELEGATE_ASSIGNMENT_OP(from)\
  DELEGATE_MOVE_ASSIGNMENT_OP(from)

#define DELEGATE1_BIG6(from,t1) \
  DELEGATE1_CONSTRUCTOR(from,t1) \
  DELEGATE_DESTRUCTOR(from) \
  DELEGATE_COPY_CONSTRUCTOR(from) \
  DELEGATE_MOVE_CONSTRUCTOR(from) \
  DELEGATE_ASSIGNMENT_OP(from)\
  DELEGATE_MOVE_ASSIGNMENT_OP(from)

#define DELEGATE2_BIG6(from,t1,t2) \
  DELEGATE2_CONSTRUCTOR(from,t1,t2) \
  DELEGATE_DESTRUCTOR(from) \
  DELEGATE_COPY_CONSTRUCTOR(from) \
  DELEGATE_MOVE_CONSTRUCTOR(from) \
  DELEGATE_ASSIGNMENT_OP(from)\
  DELEGATE_MOVE_ASSIGNMENT_OP(from)

#define DELEGATE3_BIG6(from,t1,t2,t3) \
  DELEGATE3_CONSTRUCTOR(from,t1,t2,t3) \
  DELEGATE_DESTRUCTOR(from) \
  DELEGATE_COPY_CONSTRUCTOR(from) \
  DELEGATE_MOVE_CONSTRUCTOR(from) \
  DELEGATE_ASSIGNMENT_OP(from)\
  DELEGATE_MOVE_ASSIGNMENT_OP(from)

#define DELEGATE4_BIG6(from,t1,t2,t3,t4) \
  DELEGATE4_CONSTRUCTOR(from,t1,t2,t3,t4) \
  DELEGATE_DESTRUCTOR(from) \
  DELEGATE_COPY_CONSTRUCTOR(from) \
  DELEGATE_MOVE_CONSTRUCTOR(from) \
  DELEGATE_ASSIGNMENT_OP(from)\
  DELEGATE_MOVE_ASSIGNMENT_OP(from)

#define DELEGATE5_BIG6(from,t1,t2,t3,t4,t5) \
  DELEGATE5_CONSTRUCTOR(from,t1,t2,t3,t4,t5) \
  DELEGATE_DESTRUCTOR(from) \
  DELEGATE_COPY_CONSTRUCTOR(from) \
  DELEGATE_MOVE_CONSTRUCTOR(from) \
  DELEGATE_ASSIGNMENT_OP(from)\
  DELEGATE_MOVE_ASSIGNMENT_OP(from)

#define DELEGATE(r,from,method) \
  r from :: method () { return this->impl-> method (); }

#define DELEGATE1(r,from,method,t1) \
  r from :: method (t1 a1) { \
    return this->impl-> method (a1); }

#define DELEGATE2(r,from,method,t1,t2) \
  r from :: method (t1 a1,t2 a2) { \
    return this->impl-> method (a1,a2); }

#define DELEGATE3(r,from,method,t1,t2,t3) \
  r from :: method (t1 a1,t2 a2,t3 a3) { \
    return this->impl-> method (a1,a2,a3); }

#define DELEGATE4(r,from,method,t1,t2,t3,t4) \
  r from :: method (t1 a1,t2 a2,t3 a3,t4 a4) { \
    return this->impl-> method (a1,a2,a3,a4); }

#define DELEGATE5(r,from,method,t1,t2,t3,t4,t5) \
  r from :: method (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5) { \
    return this->impl-> method (a1,a2,a3,a4,a5); }

#define DELEGATE_CONST(r,from,method) \
  r from :: method () const { \
    const Impl * constImpl = this->impl; \
    return constImpl-> method (); }

#define DELEGATE1_CONST(r,from,method,t1) \
  r from :: method (t1 a1) const { \
    const Impl * constImpl = this->impl; \
    return constImpl-> method (a1); }

#define DELEGATE2_CONST(r,from,method,t1,t2) \
  r from :: method (t1 a1,t2 a2) const { \
    const Impl * constImpl = this->impl; \
    return constImpl-> method (a1,a2); }

#define DELEGATE3_CONST(r,from,method,t1,t2,t3) \
  r from :: method (t1 a1,t2 a2,t3 a3) const { \
    const Impl * constImpl = this->impl; \
    return constImpl-> method (a1,a2,a3); }

#define DELEGATE4_CONST(r,from,method,t1,t2,t3,t4) \
  r from :: method (t1 a1,t2 a2,t3 a3,t4 a4) const { \
    const Impl * constImpl = this->impl; \
    return constImpl-> method (a1,a2,a3,a4); }

#define DELEGATE5_CONST(r,from,method,t1,t2,t3,t4,t5) \
  r from :: method (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5) { \
    const Impl * constImpl = this->impl; \
    return constImpl-> method (a1,a2,a3,a4,a5); }

#define DELEGATE_STATIC(r,from,method) \
  r from :: method () { return Impl :: method (); }

#define DELEGATE1_STATIC(r,from,method,t1) \
  r from :: method (t1 a1) { \
    return Impl :: method (a1); }

#define DELEGATE2_STATIC(r,from,method,t1,t2) \
  r from :: method (t1 a1,t2 a2) { \
    return Impl :: method (a1,a2); }

#define DELEGATE3_STATIC(r,from,method,t1,t2,t3) \
  r from :: method (t1 a1,t2 a2,t3 a3) { \
    return Impl :: method (a1,a2,a3); }

#define DELEGATE4_STATIC(r,from,method,t1,t2,t3,t4) \
  r from :: method (t1 a1,t2 a2,t3 a3,t4 a4) { \
    return Impl :: method (a1,a2,a3,a4); }

#define DELEGATE5_STATIC(r,from,method,t1,t2,t3,t4,t5) \
  r from :: method (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5) { \
    return Impl :: method (a1,a2,a3,a4,a5); }

#define GETTER_CONST(r,from,member) \
  r from :: member () const { return this->impl-> member ; }

#define GETTER(r,from,member) \
  r from :: member () { return this->impl-> member ; }

#define SETTER(t,from,member) \
  void from :: member (t a) { this->impl-> member = a; }

#define GLOBAL(from) \
  from & from :: global () { static from g; return g; }

#define DELEGATE_GLOBAL(r,from,method) \
  r from :: method () { return from :: global ().impl-> method (); }

#define DELEGATE1_GLOBAL(r,from,method,t1) \
  r from :: method (t1 a1) { \
    return from :: global ().impl-> method (a1); }

#define DELEGATE2_GLOBAL(r,from,method,t1,t2) \
  r from :: method (t1 a1,t2 a2) { \
    return from :: global ().impl-> method (a1,a2); }

#define DELEGATE3_GLOBAL(r,from,method,t1,t2,t3) \
  r from :: method (t1 a1,t2 a2,t3 a3) { \
    return from :: global ().impl-> method (a1,a2,a3); }

#define DELEGATE4_GLOBAL(r,from,method,t1,t2,t3,t4) \
  r from :: method (t1 a1,t2 a2,t3 a3,t4 a4) { \
    return from :: global ().impl-> method (a1,a2,a3,a4); }

#define DELEGATE5_GLOBAL(r,from,method,t1,t2,t3,t4,t5) \
  r from :: method (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5) { \
    return from :: global ().impl-> method (a1,a2,a3,a4,a5); }

#define ID(from) \
  Id from :: id () const { return this->impl->id.id () ; }

// Safe references

#define SAFE_REF(r,method) \
  r & method ## Ref () { \
    r * ptr = this-> method (); \
    assert (ptr); \
    return *ptr; }

#define SAFE_REF1(r,method,t1) \
  r & method ## Ref (t1 a1) { \
    r * ptr = this-> method (a1); \
    assert (ptr); \
    return *ptr; }

#define SAFE_REF2(r,method,t1,t2) \
  r & method ## Ref (t1 a1,t2 a2) { \
    r * ptr = this-> method (a1,a2); \
    assert (ptr); \
    return *ptr; }

#define SAFE_REF3(r,method,t1,t2,t3) \
  r & method ## Ref (t1 a1,t2 a2,t3 a3) { \
    r * ptr = this-> method (a1,a2,a3); \
    assert (ptr); \
    return *ptr; }

#define SAFE_REF4(r,method,t1,t2,t3,t4) \
  r & method ## Ref (t1 a1,t2 a2,t3 a3,t4 a4) { \
    r * ptr = this-> method (a1,a2,a3,a4); \
    assert (ptr); \
    return *ptr; }

#define SAFE_REF5(r,method,t1,t2,t3,t4,t5) \
  r & method ## Ref (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5) { \
    r * ptr = this-> method (a1,a2,a3,a4,a5); \
    assert (ptr); \
    return *ptr; }

#define SAFE_REF_CONST(r,method) \
  r & method ## Ref () const { \
    r * ptr = this-> method (); \
    assert (ptr); \
    return *ptr; }

#define SAFE_REF1_CONST(r,method,t1) \
  r & method ## Ref (t1 a1) const { \
    r * ptr = this-> method (a1); \
    assert (ptr); \
    return *ptr; }

#define SAFE_REF2_CONST(r,method,t1,t2) \
  r & method ## Ref (t1 a1,t2 a2) const { \
    r * ptr = this-> method (a1,a2); \
    assert (ptr); \
    return *ptr; }

#define SAFE_REF3_CONST(r,method,t1,t2,t3) \
  r & method ## Ref (t1 a1,t2 a2,t3 a3) const { \
    r * ptr = this-> method (a1,a2,a3); \
    assert (ptr); \
    return *ptr; }

#define SAFE_REF4_CONST(r,method,t1,t2,t3,t4) \
  r & method ## Ref (t1 a1,t2 a2,t3 a3,t4 a4) const { \
    r * ptr = this-> method (a1,a2,a3,a4); \
    assert (ptr); \
    return *ptr; }

#define SAFE_REF5_CONST(r,method,t1,t2,t3,t4,t5) \
  r & method ## Ref (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5) const { \
    r * ptr = this-> method (a1,a2,a3,a4,a5); \
    assert (ptr); \
    return *ptr; }

// Big 6 declarations

#define DECLARE_BIG6(t,...)              \
         t             (__VA_ARGS__);    \
         t             (const t &);      \
         t             (      t &&);     \
  const  t & operator= (const t &);      \
  const  t & operator= (      t &&);     \
       ~ t             ();

// Action related macros

#define DECLARE_ACTION_BIG6(t,...)            \
         t             (__VA_ARGS__);         \
         t             (const t &)  = delete; \
         t             (      t &&);          \
  const  t & operator= (const t &)  = delete; \
  const  t & operator= (      t &&) = delete; \
       ~ t             ();

#define DELEGATE_ACTION_BIG6(from) \
  DELEGATE_CONSTRUCTOR(from) \
  DELEGATE_DESTRUCTOR(from) \
  DELEGATE_MOVE_CONSTRUCTOR(from)

#define DELEGATE_TO_UNIT_TEMPLATE(from,t) \
  void from :: addAction (t * a, SubActionKind kind) { \
    this->impl->unitTemplate.addAction (a, kind); } \
  void from :: reset () { \
    this->impl->unitTemplate.reset (); }

#endif

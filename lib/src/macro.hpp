#ifndef DILAY_MACRO
#define DILAY_MACRO

#include <cassert>
#include <memory>

// delegators

#define DELEGATE_COPY_CONSTRUCTOR(from) \
  from :: from (const from & a1) : impl (new Impl (*a1.impl)) {}

#define DELEGATE_COPY_CONSTRUCTOR_SELF(from) \
  from :: from (const from & a1) : impl (new Impl (*a1.impl)) { \
    this->impl->self = this; }

#define DELEGATE_COPY_CONSTRUCTOR_BASE(from,base) \
  from :: from (const from & a1) : base (a1), impl (new Impl (*a1.impl)) { \
    this->impl->self = this; }

#define DELEGATE_MOVE_CONSTRUCTOR(from) \
  from :: from (from && a1) : impl (new Impl (std::move (*a1.impl))) {}

#define DELEGATE_MOVE_CONSTRUCTOR_SELF(from) \
  from :: from (from && a1) : impl (new Impl (std::move (*a1.impl))) { \
    this->impl->self = this; }

#define DELEGATE_MOVE_CONSTRUCTOR_BASE(from,base) \
  from :: from (from && a1) : base (std::forward < base > (a1)) \
                            , impl (new Impl (std::move (*a1.impl))) \
    { this->impl->self = this; }

#define DELEGATE_ASSIGNMENT_OP(from) \
  const from & from :: operator= (const from & source) { \
    this->impl->operator= (*source.impl); \
    return *this; }

#define DELEGATE_MOVE_ASSIGNMENT_OP(from) \
  const from & from :: operator= (from && source) { \
    this->impl->operator= (std::move (*source.impl)); \
    return *this; }

#define DELEGATE_DESTRUCTOR(from) from :: ~ from () {}

#define DELEGATE_BIG3_WITHOUT_CONSTRUCTOR(from) \
  DELEGATE_DESTRUCTOR(from) \
  DELEGATE_MOVE_CONSTRUCTOR(from)

#define DELEGATE_BIG3_WITHOUT_CONSTRUCTOR_SELF(from) \
  DELEGATE_DESTRUCTOR(from) \
  DELEGATE_MOVE_CONSTRUCTOR_SELF(from)

#define DELEGATE_BIG4MOVE_WITHOUT_CONSTRUCTOR(from) \
  DELEGATE_BIG3_WITHOUT_CONSTRUCTOR(from) \
  DELEGATE_MOVE_ASSIGNMENT_OP(from)

#define DELEGATE_BIG4MOVE_WITHOUT_CONSTRUCTOR_SELF(from) \
  DELEGATE_BIG3_WITHOUT_CONSTRUCTOR_SELF(from) \
  DELEGATE_MOVE_ASSIGNMENT_OP(from)

#define DELEGATE_BIG4COPY_WITHOUT_CONSTRUCTOR(from) \
  DELEGATE_BIG3_WITHOUT_CONSTRUCTOR(from) \
  DELEGATE_COPY_CONSTRUCTOR(from)

#define DELEGATE_BIG4COPY_WITHOUT_CONSTRUCTOR_SELF(from) \
  DELEGATE_BIG3_WITHOUT_CONSTRUCTOR_SELF(from) \
  DELEGATE_COPY_CONSTRUCTOR_SELF(from)

#define DELEGATE_BIG6_WITHOUT_CONSTRUCTOR(from) \
  DELEGATE_BIG3_WITHOUT_CONSTRUCTOR(from) \
  DELEGATE_MOVE_ASSIGNMENT_OP(from) \
  DELEGATE_COPY_CONSTRUCTOR(from) \
  DELEGATE_ASSIGNMENT_OP(from) 

#define DELEGATE_BIG6_WITHOUT_CONSTRUCTOR_SELF(from) \
  DELEGATE_BIG3_WITHOUT_CONSTRUCTOR_SELF(from) \
  DELEGATE_MOVE_ASSIGNMENT_OP(from) \
  DELEGATE_COPY_CONSTRUCTOR_SELF(from) \
  DELEGATE_ASSIGNMENT_OP(from)

#define DELEGATE_CONSTRUCTOR(from) \
  from :: from () : impl (new Impl ()) {}

#define DELEGATE1_CONSTRUCTOR(from,t1) \
  from :: from (t1 a1) : impl (new Impl (a1)) {}

#define DELEGATE2_CONSTRUCTOR(from,t1,t2) \
  from :: from (t1 a1,t2 a2) : impl (new Impl (a1,a2)) {}

#define DELEGATE3_CONSTRUCTOR(from,t1,t2,t3) \
  from :: from (t1 a1,t2 a2,t3 a3) : impl (new Impl (a1,a2,a3)) {}

#define DELEGATE4_CONSTRUCTOR(from,t1,t2,t3,t4) \
  from :: from (t1 a1,t2 a2,t3 a3,t4 a4) : impl (new Impl (a1,a2,a3,a4)) {}

#define DELEGATE5_CONSTRUCTOR(from,t1,t2,t3,t4,t5) \
  from :: from (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5) : impl (new Impl (a1,a2,a3,a4,a5)) {}

#define DELEGATE_CONSTRUCTOR_SELF(from) \
  from :: from () : impl (new Impl (this)) {}

#define DELEGATE1_CONSTRUCTOR_SELF(from,t1) \
  from :: from (t1 a1) : impl (new Impl (this,a1)) {}

#define DELEGATE2_CONSTRUCTOR_SELF(from,t1,t2) \
  from :: from (t1 a1,t2 a2) : impl (new Impl (this,a1,a2)) {}

#define DELEGATE3_CONSTRUCTOR_SELF(from,t1,t2,t3) \
  from :: from (t1 a1,t2 a2,t3 a3) : impl (new Impl (this,a1,a2,a3)) {}

#define DELEGATE4_CONSTRUCTOR_SELF(from,t1,t2,t3,t4) \
  from :: from (t1 a1,t2 a2,t3 a3,t4 a4) : impl (new Impl (this,a1,a2,a3,a4)) {}

#define DELEGATE5_CONSTRUCTOR_SELF(from,t1,t2,t3,t4,t5) \
  from :: from (t1 a1,t2 a2,t3 a3,t4 a4,t5 a5) : impl (new Impl (this,a1,a2,a3,a4,a5)) {}

#define DELEGATE_CONSTRUCTOR_BASE(from,params,fromArgs,base,baseArgs) \
  from :: from params : base baseArgs, impl (new Impl fromArgs ) {}

#define DELEGATE_BASE(r,from,method,ifaceParams,implArgs) \
  r from :: method ifaceParams { return this->impl-> method implArgs ; }

#define DELEGATE(r,from,method) \
  DELEGATE_BASE (r,from,method,(),())

#define DELEGATE1(r,from,method,t1) \
  DELEGATE_BASE (r,from,method,(t1 a1),( std::forward<t1>(a1) ))

#define DELEGATE2(r,from,method,t1,t2) \
  DELEGATE_BASE (r,from,method,(t1 a1,t2 a2), ( std::forward<t1>(a1) \
                                              , std::forward<t2>(a2) ))

#define DELEGATE3(r,from,method,t1,t2,t3) \
  DELEGATE_BASE (r,from,method,(t1 a1,t2 a2,t3 a3), ( std::forward<t1>(a1) \
                                                    , std::forward<t2>(a2) \
                                                    , std::forward<t3>(a3) ))

#define DELEGATE4(r,from,method,t1,t2,t3,t4) \
  DELEGATE_BASE (r,from,method,(t1 a1,t2 a2,t3 a3,t4 a4), ( std::forward<t1>(a1) \
                                                          , std::forward<t2>(a2) \
                                                          , std::forward<t3>(a3) \
                                                          , std::forward<t4>(a4) ))

#define DELEGATE5(r,from,method,t1,t2,t3,t4,t5) \
  DELEGATE_BASE (r,from,method,(t1 a1,t2 a2,t3 a3,t4 a4,t5 a5), ( std::forward<t1>(a1) \
                                                                , std::forward<t2>(a2) \
                                                                , std::forward<t3>(a3) \
                                                                , std::forward<t4>(a4) \
                                                                , std::forward<t5>(a5) ))

#define DELEGATE_BASE_CONST(r,from,method,ifaceParams,implArgs) \
  r from :: method ifaceParams const { return this->impl-> method implArgs ; }

#define DELEGATE_CONST(r,from,method) \
  DELEGATE_BASE_CONST (r,from,method,(),())

#define DELEGATE1_CONST(r,from,method,t1) \
  DELEGATE_BASE_CONST (r,from,method,(t1 a1), ( std::forward<t1>(a1) ))

#define DELEGATE2_CONST(r,from,method,t1,t2) \
  DELEGATE_BASE_CONST (r,from,method,(t1 a1,t2 a2), ( std::forward<t1>(a1) \
                                                    , std::forward<t2>(a2) ))

#define DELEGATE3_CONST(r,from,method,t1,t2,t3) \
  DELEGATE_BASE_CONST (r,from,method,(t1 a1,t2 a2,t3 a3), ( std::forward<t1>(a1) \
                                                          , std::forward<t2>(a2) \
                                                          , std::forward<t3>(a3) ))

#define DELEGATE4_CONST(r,from,method,t1,t2,t3,t4) \
  DELEGATE_BASE_CONST (r,from,method,(t1 a1,t2 a2,t3 a3,t4 a4), ( std::forward<t1>(a1) \
                                                                , std::forward<t2>(a2) \
                                                                , std::forward<t3>(a3) \
                                                                , std::forward<t4>(a4) ))

#define DELEGATE5_CONST(r,from,method,t1,t2,t3,t4,t5) \
  DELEGATE_BASE_CONST (r,from,method,(t1 a1,t2 a2,t3 a3,t4 a4,t5 a5), ( std::forward<t1>(a1) \
                                                                      , std::forward<t2>(a2) \
                                                                      , std::forward<t3>(a3) \
                                                                      , std::forward<t4>(a4) \
                                                                      , std::forward<t5>(a5) ))

#define DELEGATE_BASE_STATIC(r,from,method,ifaceParams,implArgs) \
  r from :: method ifaceParams { return Impl :: method implArgs ; }

#define DELEGATE_STATIC(r,from,method) \
  DELEGATE_BASE_STATIC (r,from,method,(),())

#define DELEGATE1_STATIC(r,from,method,t1) \
  DELEGATE_BASE_STATIC (r,from,method,(t1 a1),(a1))

#define DELEGATE2_STATIC(r,from,method,t1,t2) \
  DELEGATE_BASE_STATIC (r,from,method,(t1 a1,t2 a2),(a1,a2))

#define DELEGATE3_STATIC(r,from,method,t1,t2,t3) \
  DELEGATE_BASE_STATIC (r,from,method,(t1 a1,t2 a2,t3 a3),(a1,a2,a3))

#define DELEGATE4_STATIC(r,from,method,t1,t2,t3,t4) \
  DELEGATE_BASE_STATIC (r,from,method,(t1 a1,t2 a2,t3 a3,t4 a4),(a1,a2,a3,a4))

#define DELEGATE5_STATIC(r,from,method,t1,t2,t3,t4,t5) \
  DELEGATE_BASE_STATIC (r,from,method,(t1 a1,t2 a2,t3 a3,t4 a4,t5 a5),(a1,a2,a3,a4,a5))

#define GLOBAL(from) \
  from & from :: global () { static from g; return g; }

#define DELEGATE_BASE_GLOBAL(r,from,method,ifaceParams,implArgs) \
  r from :: method ifaceParams { return from :: global ().impl-> method implArgs ; }

#define DELEGATE_GLOBAL(r,from,method) \
  DELEGATE_BASE_GLOBAL (r,from,method,(),())

#define DELEGATE1_GLOBAL(r,from,method,t1) \
  DELEGATE_BASE_GLOBAL (r,from,method,(t1 a1),(a1))

#define DELEGATE2_GLOBAL(r,from,method,t1,t2) \
  DELEGATE_BASE_GLOBAL (r,from,method,(t1 a1,t2 a2),(a1,a2))

#define DELEGATE3_GLOBAL(r,from,method,t1,t2,t3) \
  DELEGATE_BASE_GLOBAL (r,from,method,(t1 a1,t2 a2,t3 a3),(a1,a2,a3))

#define DELEGATE4_GLOBAL(r,from,method,t1,t2,t3,t4) \
  DELEGATE_BASE_GLOBAL (r,from,method,(t1 a1,t2 a2,t3 a3,t4 a4),(a1,a2,a3,a4))

#define DELEGATE5_GLOBAL(r,from,method,t1,t2,t3,t4,t5) \
  DELEGATE_BASE_GLOBAL (r,from,method,(t1 a1,t2 a2,t3 a3,t4 a4,t5 a5),(a1,a2,a3,a4,a5))

// big 2 delegators

#define DELEGATE_BIG2(from) \
  DELEGATE_CONSTRUCTOR(from) \
  DELEGATE_DESTRUCTOR(from)

#define DELEGATE1_BIG2(from,t1) \
  DELEGATE1_CONSTRUCTOR(from,t1) \
  DELEGATE_DESTRUCTOR(from)

#define DELEGATE2_BIG2(from,t1,t2) \
  DELEGATE2_CONSTRUCTOR(from,t1,t2) \
  DELEGATE_DESTRUCTOR(from)

#define DELEGATE3_BIG2(from,t1,t2,t3) \
  DELEGATE3_CONSTRUCTOR(from,t1,t2,t3) \
  DELEGATE_DESTRUCTOR(from)

#define DELEGATE4_BIG2(from,t1,t2,t3,t4) \
  DELEGATE4_CONSTRUCTOR(from,t1,t2,t3,t4) \
  DELEGATE_DESTRUCTOR(from)

#define DELEGATE5_BIG2(from,t1,t2,t3,t4,t5) \
  DELEGATE5_CONSTRUCTOR(from,t1,t2,t3,t4,t5) \
  DELEGATE_DESTRUCTOR(from)

#define DELEGATE_BIG2_SELF(from) \
  DELEGATE_CONSTRUCTOR_SELF(from) \
  DELEGATE_DESTRUCTOR(from)

#define DELEGATE1_BIG2_SELF(from,t1) \
  DELEGATE1_CONSTRUCTOR_SELF(from,t1) \
  DELEGATE_DESTRUCTOR(from)

#define DELEGATE2_BIG2_SELF(from,t1,t2) \
  DELEGATE2_CONSTRUCTOR_SELF(from,t1,t2) \
  DELEGATE_DESTRUCTOR(from)

#define DELEGATE3_BIG2_SELF(from,t1,t2,t3) \
  DELEGATE3_CONSTRUCTOR_SELF(from,t1,t2,t3) \
  DELEGATE_DESTRUCTOR(from)

#define DELEGATE4_BIG2_SELF(from,t1,t2,t3,t4) \
  DELEGATE4_CONSTRUCTOR_SELF(from,t1,t2,t3,t4) \
  DELEGATE_DESTRUCTOR(from)

#define DELEGATE5_BIG2_SELF(from,t1,t2,t3,t4,t5) \
  DELEGATE5_CONSTRUCTOR_SELF(from,t1,t2,t3,t4,t5) \
  DELEGATE_DESTRUCTOR(from)

#define DELEGATE_BIG2_BASE(from,params,fromArgs,base,baseArgs) \
  DELEGATE_CONSTRUCTOR_BASE(from,params,fromArgs,base,baseArgs) \
  DELEGATE_DESTRUCTOR(from)

// big 3 delegators

#define DELEGATE_BIG3(from) \
  DELEGATE_CONSTRUCTOR(from) \
  DELEGATE_BIG3_WITHOUT_CONSTRUCTOR(from)

#define DELEGATE1_BIG3(from,t1) \
  DELEGATE1_CONSTRUCTOR(from,t1) \
  DELEGATE_BIG3_WITHOUT_CONSTRUCTOR(from)

#define DELEGATE2_BIG3(from,t1,t2) \
  DELEGATE2_CONSTRUCTOR(from,t1,t2) \
  DELEGATE_BIG3_WITHOUT_CONSTRUCTOR(from)

#define DELEGATE3_BIG3(from,t1,t2,t3) \
  DELEGATE3_CONSTRUCTOR(from,t1,t2,t3) \
  DELEGATE_BIG3_WITHOUT_CONSTRUCTOR(from)

#define DELEGATE4_BIG3(from,t1,t2,t3,t4) \
  DELEGATE4_CONSTRUCTOR(from,t1,t2,t3,t4) \
  DELEGATE_BIG3_WITHOUT_CONSTRUCTOR(from)

#define DELEGATE5_BIG3(from,t1,t2,t3,t4,t5) \
  DELEGATE5_CONSTRUCTOR(from,t1,t2,t3,t4,t5) \
  DELEGATE_BIG3_WITHOUT_CONSTRUCTOR(from)

#define DELEGATE_BIG3_SELF(from) \
  DELEGATE_CONSTRUCTOR_SELF(from) \
  DELEGATE_BIG3_WITHOUT_CONSTRUCTOR_SELF(from)

#define DELEGATE1_BIG3_SELF(from,t1) \
  DELEGATE1_CONSTRUCTOR_SELF(from,t1) \
  DELEGATE_BIG3_WITHOUT_CONSTRUCTOR_SELF(from)

#define DELEGATE2_BIG3_SELF(from,t1,t2) \
  DELEGATE2_CONSTRUCTOR_SELF(from,t1,t2) \
  DELEGATE_BIG3_WITHOUT_CONSTRUCTOR_SELF(from)

#define DELEGATE3_BIG3_SELF(from,t1,t2,t3) \
  DELEGATE3_CONSTRUCTOR_SELF(from,t1,t2,t3) \
  DELEGATE_BIG3_WITHOUT_CONSTRUCTOR_SELF(from)

#define DELEGATE4_BIG3_SELF(from,t1,t2,t3,t4) \
  DELEGATE4_CONSTRUCTOR_SELF(from,t1,t2,t3,t4) \
  DELEGATE_BIG3_WITHOUT_CONSTRUCTOR_SELF(from)

#define DELEGATE5_BIG3_SELF(from,t1,t2,t3,t4,t5) \
  DELEGATE5_CONSTRUCTOR_SELF(from,t1,t2,t3,t4,t5) \
  DELEGATE_BIG3_WITHOUT_CONSTRUCTOR_SELF(from)

#define DELEGATE_BIG3_BASE(from,params,fromArgs,base,baseArgs) \
  DELEGATE_CONSTRUCTOR_BASE(from,params,fromArgs,base,baseArgs) \
  DELEGATE_DESTRUCTOR(from) \
  DELEGATE_MOVE_CONSTRUCTOR_BASE(from,base)

// big 4 (move) delegators

#define DELEGATE_BIG4MOVE(from) \
  DELEGATE_CONSTRUCTOR(from) \
  DELEGATE_BIG4MOVE_WITHOUT_CONSTRUCTOR(from)

#define DELEGATE1_BIG4MOVE(from,t1) \
  DELEGATE1_CONSTRUCTOR(from,t1) \
  DELEGATE_BIG4MOVE_WITHOUT_CONSTRUCTOR(from)

#define DELEGATE2_BIG4MOVE(from,t1,t2) \
  DELEGATE2_CONSTRUCTOR(from,t1,t2) \
  DELEGATE_BIG4MOVE_WITHOUT_CONSTRUCTOR(from)

#define DELEGATE3_BIG4MOVE(from,t1,t2,t3) \
  DELEGATE3_CONSTRUCTOR(from,t1,t2,t3) \
  DELEGATE_BIG4MOVE_WITHOUT_CONSTRUCTOR(from)

#define DELEGATE4_BIG4MOVE(from,t1,t2,t3,t4) \
  DELEGATE4_CONSTRUCTOR(from,t1,t2,t3,t4) \
  DELEGATE_BIG4MOVE_WITHOUT_CONSTRUCTOR(from)

#define DELEGATE5_BIG4MOVE(from,t1,t2,t3,t4,t5) \
  DELEGATE5_CONSTRUCTOR(from,t1,t2,t3,t4,t5) \
  DELEGATE_BIG4MOVE_WITHOUT_CONSTRUCTOR(from)

#define DELEGATE_BIG4MOVE_SELF(from) \
  DELEGATE_CONSTRUCTOR_SELF(from) \
  DELEGATE_BIG4MOVE_WITHOUT_CONSTRUCTOR_SELF(from)

#define DELEGATE1_BIG4MOVE_SELF(from,t1) \
  DELEGATE1_CONSTRUCTOR_SELF(from,t1) \
  DELEGATE_BIG4MOVE_WITHOUT_CONSTRUCTOR_SELF(from)

#define DELEGATE2_BIG4MOVE_SELF(from,t1,t2) \
  DELEGATE2_CONSTRUCTOR_SELF(from,t1,t2) \
  DELEGATE_BIG4MOVE_WITHOUT_CONSTRUCTOR_SELF(from)

#define DELEGATE3_BIG4MOVE_SELF(from,t1,t2,t3) \
  DELEGATE3_CONSTRUCTOR_SELF(from,t1,t2,t3) \
  DELEGATE_BIG4MOVE_WITHOUT_CONSTRUCTOR_SELF(from)

#define DELEGATE4_BIG4MOVE_SELF(from,t1,t2,t3,t4) \
  DELEGATE4_CONSTRUCTOR_SELF(from,t1,t2,t3,t4) \
  DELEGATE_BIG4MOVE_WITHOUT_CONSTRUCTOR_SELF(from)

#define DELEGATE5_BIG4MOVE_SELF(from,t1,t2,t3,t4,t5) \
  DELEGATE5_CONSTRUCTOR_SELF(from,t1,t2,t3,t4,t5) \
  DELEGATE_BIG4MOVE_WITHOUT_CONSTRUCTOR_SELF(from)

#define DELEGATE_BIG4MOVE_BASE(from,params,fromArgs,base,baseArgs) \
  DELEGATE_CONSTRUCTOR_BASE(from,params,fromArgs,base,baseArgs) \
  DELEGATE_DESTRUCTOR(from) \
  DELEGATE_MOVE_CONSTRUCTOR_BASE(from,base) \
  DELEGATE_MOVE_ASSIGNMENT_OP(from)

// big 4 (copy) delegators

#define DELEGATE_BIG4COPY(from) \
  DELEGATE_CONSTRUCTOR(from) \
  DELEGATE_BIG4COPY_WITHOUT_CONSTRUCTOR(from)

#define DELEGATE1_BIG4COPY(from,t1) \
  DELEGATE1_CONSTRUCTOR(from,t1) \
  DELEGATE_BIG4COPY_WITHOUT_CONSTRUCTOR(from)

#define DELEGATE2_BIG4COPY(from,t1,t2) \
  DELEGATE2_CONSTRUCTOR(from,t1,t2) \
  DELEGATE_BIG4COPY_WITHOUT_CONSTRUCTOR(from)

#define DELEGATE3_BIG4COPY(from,t1,t2,t3) \
  DELEGATE3_CONSTRUCTOR(from,t1,t2,t3) \
  DELEGATE_BIG4COPY_WITHOUT_CONSTRUCTOR(from)

#define DELEGATE4_BIG4COPY(from,t1,t2,t3,t4) \
  DELEGATE4_CONSTRUCTOR(from,t1,t2,t3,t4) \
  DELEGATE_BIG4COPY_WITHOUT_CONSTRUCTOR(from)

#define DELEGATE5_BIG4COPY(from,t1,t2,t3,t4,t5) \
  DELEGATE5_CONSTRUCTOR(from,t1,t2,t3,t4,t5) \
  DELEGATE_BIG4COPY_WITHOUT_CONSTRUCTOR(from)

#define DELEGATE_BIG4COPY_SELF(from) \
  DELEGATE_CONSTRUCTOR_SELF(from) \
  DELEGATE_BIG4COPY_WITHOUT_CONSTRUCTOR_SELF(from)

#define DELEGATE1_BIG4COPY_SELF(from,t1) \
  DELEGATE1_CONSTRUCTOR_SELF(from,t1) \
  DELEGATE_BIG4COPY_WITHOUT_CONSTRUCTOR_SELF(from)

#define DELEGATE2_BIG4COPY_SELF(from,t1,t2) \
  DELEGATE2_CONSTRUCTOR_SELF(from,t1,t2) \
  DELEGATE_BIG4COPY_WITHOUT_CONSTRUCTOR_SELF(from)

#define DELEGATE3_BIG4COPY_SELF(from,t1,t2,t3) \
  DELEGATE3_CONSTRUCTOR_SELF(from,t1,t2,t3) \
  DELEGATE_BIG4COPY_WITHOUT_CONSTRUCTOR_SELF(from)

#define DELEGATE4_BIG4COPY_SELF(from,t1,t2,t3,t4) \
  DELEGATE4_CONSTRUCTOR_SELF(from,t1,t2,t3,t4) \
  DELEGATE_BIG4COPY_WITHOUT_CONSTRUCTOR_SELF(from)

#define DELEGATE5_BIG4COPY_SELF(from,t1,t2,t3,t4,t5) \
  DELEGATE5_CONSTRUCTOR_SELF(from,t1,t2,t3,t4,t5) \
  DELEGATE_BIG4COPY_WITHOUT_CONSTRUCTOR_SELF(from)

#define DELEGATE_BIG4COPY_BASE(from,params,fromArgs,base,baseArgs) \
  DELEGATE_CONSTRUCTOR_BASE(from,params,fromArgs,base,baseArgs) \
  DELEGATE_DESTRUCTOR(from) \
  DELEGATE_MOVE_CONSTRUCTOR_BASE(from,base) \
  DELEGATE_COPY_CONSTRUCTOR_BASE(from,base)

// big 6 delegators

#define DELEGATE_BIG6(from) \
  DELEGATE_CONSTRUCTOR(from) \
  DELEGATE_BIG6_WITHOUT_CONSTRUCTOR(from)

#define DELEGATE1_BIG6(from,t1) \
  DELEGATE1_CONSTRUCTOR(from,t1) \
  DELEGATE_BIG6_WITHOUT_CONSTRUCTOR(from)

#define DELEGATE2_BIG6(from,t1,t2) \
  DELEGATE2_CONSTRUCTOR(from,t1,t2) \
  DELEGATE_BIG6_WITHOUT_CONSTRUCTOR(from)

#define DELEGATE3_BIG6(from,t1,t2,t3) \
  DELEGATE3_CONSTRUCTOR(from,t1,t2,t3) \
  DELEGATE_BIG6_WITHOUT_CONSTRUCTOR(from)

#define DELEGATE4_BIG6(from,t1,t2,t3,t4) \
  DELEGATE4_CONSTRUCTOR(from,t1,t2,t3,t4) \
  DELEGATE_BIG6_WITHOUT_CONSTRUCTOR(from)

#define DELEGATE5_BIG6(from,t1,t2,t3,t4,t5) \
  DELEGATE5_CONSTRUCTOR(from,t1,t2,t3,t4,t5) \
  DELEGATE_BIG6_WITHOUT_CONSTRUCTOR(from)

#define DELEGATE_BIG6_SELF(from) \
  DELEGATE_CONSTRUCTOR_SELF(from) \
  DELEGATE_BIG6_WITHOUT_CONSTRUCTOR_SELF(from)

#define DELEGATE1_BIG6_SELF(from,t1) \
  DELEGATE1_CONSTRUCTOR_SELF(from,t1) \
  DELEGATE_BIG6_WITHOUT_CONSTRUCTOR_SELF(from)

#define DELEGATE2_BIG6_SELF(from,t1,t2) \
  DELEGATE2_CONSTRUCTOR_SELF(from,t1,t2) \
  DELEGATE_BIG6_WITHOUT_CONSTRUCTOR_SELF(from)

#define DELEGATE3_BIG6_SELF(from,t1,t2,t3) \
  DELEGATE3_CONSTRUCTOR_SELF(from,t1,t2,t3) \
  DELEGATE_BIG6_WITHOUT_CONSTRUCTOR_SELF(from)

#define DELEGATE4_BIG6_SELF(from,t1,t2,t3,t4) \
  DELEGATE4_CONSTRUCTOR_SELF(from,t1,t2,t3,t4) \
  DELEGATE_BIG6_WITHOUT_CONSTRUCTOR_SELF(from)

#define DELEGATE5_BIG6_SELF(from,t1,t2,t3,t4,t5) \
  DELEGATE5_CONSTRUCTOR_SELF(from,t1,t2,t3,t4,t5) \
  DELEGATE_BIG6_WITHOUT_CONSTRUCTOR_SELF(from)

#define DELEGATE_BIG6_BASE(from,params,fromArgs,base,baseArgs) \
  DELEGATE_CONSTRUCTOR_BASE(from,params,fromArgs,base,baseArgs) \
  DELEGATE_DESTRUCTOR(from) \
  DELEGATE_COPY_CONSTRUCTOR_BASE(from,base) \
  DELEGATE_MOVE_CONSTRUCTOR_BASE(from,base) \
  DELEGATE_ASSIGNMENT_OP(from) \
  DELEGATE_MOVE_ASSIGNMENT_OP(from)

// getters/setters

#define GETTER_CONST(r,from,member) \
  r from :: member () const { return this->impl-> member ; }

#define GETTER(r,from,member) \
  r from :: member () { return this->impl-> member ; }

#define SETTER(t,from,member) \
  void from :: member (t a) { this->impl-> member = a; }

#define GETTER_CONST_GLOBAL(r,from,member) \
  r from :: member () const { \
    return from :: global ().impl-> member ; }

#define GETTER_GLOBAL(r,from,member) \
  r from :: member () { \
    return from :: global ().impl-> member ; }

#define SETTER_GLOBAL(t,from,member) \
  void from :: member (t a) { \
    from :: global ().impl -> member = a; }

// safe references

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

// big 2 declarations

#define DECLARE_BIG2(t,...)                   \
         t             (__VA_ARGS__);         \
         t             (const t &)  = delete; \
         t             (      t &&) = delete; \
  const  t & operator= (const t &)  = delete; \
  const  t & operator= (      t &&) = delete; \
       ~ t             ();

#define DECLARE_BIG2_VIRTUAL(t,...)                   \
                 t             (__VA_ARGS__);         \
                 t             (const t &)  = delete; \
                 t             (      t &&) = delete; \
          const  t & operator= (const t &)  = delete; \
          const  t & operator= (      t &&) = delete; \
  virtual      ~ t             ();

// big 3 declarations

#define DECLARE_BIG3(t,...)                   \
         t             (__VA_ARGS__);         \
         t             (const t &)  = delete; \
         t             (      t &&);          \
  const  t & operator= (const t &)  = delete; \
  const  t & operator= (      t &&) = delete; \
       ~ t             ();

#define DECLARE_BIG3_VIRTUAL(t,...)                   \
                 t             (__VA_ARGS__);         \
                 t             (const t &)  = delete; \
                 t             (      t &&);          \
          const  t & operator= (const t &)  = delete; \
          const  t & operator= (      t &&) = delete; \
  virtual      ~ t             ();

// big 4 (move) declarations

#define DECLARE_BIG4MOVE(t,...)               \
         t             (__VA_ARGS__);         \
         t             (const t &)  = delete; \
         t             (      t &&);          \
  const  t & operator= (const t &)  = delete; \
  const  t & operator= (      t &&);          \
       ~ t             ();

#define DECLARE_BIG4MOVE_VIRTUAL(t,...)               \
                 t             (__VA_ARGS__);         \
                 t             (const t &)  = delete; \
                 t             (      t &&);          \
          const  t & operator= (const t &)  = delete; \
          const  t & operator= (      t &&);          \
  virtual      ~ t             ();

// big 4 (copy) declarations

#define DECLARE_BIG4COPY(t,...)               \
         t             (__VA_ARGS__);         \
         t             (const t &);           \
         t             (      t &&);          \
  const  t & operator= (const t &)  = delete; \
  const  t & operator= (      t &&) = delete; \
       ~ t             ();

#define DECLARE_BIG4COPY_VIRTUAL(t,...)               \
                 t             (__VA_ARGS__);         \
                 t             (const t &);           \
                 t             (      t &&);          \
          const  t & operator= (const t &)  = delete; \
          const  t & operator= (      t &&) = delete; \
  virtual      ~ t             ();

// big 6 declarations

#define DECLARE_BIG6(t,...)              \
         t             (__VA_ARGS__);    \
         t             (const t &);      \
         t             (      t &&);     \
  const  t & operator= (const t &);      \
  const  t & operator= (      t &&);     \
       ~ t             ();

#define DECLARE_BIG6_VIRTUAL(t,...)              \
                 t             (__VA_ARGS__);    \
                 t             (const t &);      \
                 t             (      t &&);     \
          const  t & operator= (const t &);      \
          const  t & operator= (      t &&);     \
  virtual      ~ t             ();

// miscellaneous

#define DELETE_COPYMOVEASSIGN(t)              \
         t             (const t &)  = delete; \
         t             (      t &&) = delete; \
  const  t & operator= (const t &)  = delete; \
  const  t & operator= (      t &&) = delete;

#define DEFAULT_COPYMOVEASSIGN(t)              \
         t             (const t &)  = default; \
         t             (      t &&) = default; \
  const  t & operator= (const t &)  = default; \
  const  t & operator= (      t &&) = default;

#define IMPLEMENTATION  \
  class Impl; \
  std::unique_ptr <Impl> impl;

#endif

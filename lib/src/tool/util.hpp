#ifndef DILAY_TOOL_UTIL
#define DILAY_TOOL_UTIL

#include "macro.hpp"

class Tool;
class ViewToolOptions;

class ToolUtil {
  public:
    DECLARE_BIG3_VIRTUAL (ToolUtil, Tool&)

  protected:
    Tool&            tool        ();
    ViewToolOptions* toolOptions ();

  private:
    class Impl;
    Impl* impl;
};

#define DECLARE_TOOL_UTIL(t,...)                              \
  DECLARE_BIG3 ( t , Tool&, ##__VA_ARGS__)                    \

#define DELEGATE_TOOL_UTIL(t)                                 \
  t :: t (Tool& tool)                                         \
    : ToolUtil (tool) {                                       \
        DELEGATE_NEW_IMPL (this)                              \
    }                                                         \
  DELEGATE_BIG3_WITHOUT_CONSTRUCTOR_SELF ( t )                \


#define DELEGATE1_TOOL_UTIL(t,t1)                             \
  t :: t ( Tool& tool, t1 a1 )                                \
    : ToolUtil (tool) {                                       \
        DELEGATE_NEW_IMPL (this,a1)                           \
    }                                                         \
  DELEGATE_BIG3_WITHOUT_CONSTRUCTOR_SELF ( t )                \

#define DELEGATE2_TOOL_UTIL(t,t1,t2)                          \
  t :: t ( Tool& tool, t1 a1, t2 a2 )                         \
    : ToolUtil (tool) {                                       \
        DELEGATE_NEW_IMPL (this,a1,a2)                        \
    }                                                         \
  DELEGATE_BIG3_WITHOUT_CONSTRUCTOR_SELF ( t )                \

#define DELEGATE3_TOOL_UTIL(t,t1,t2,t3)                       \
  t :: t ( Tool& tool, t1 a1, t2 a2, t3 a3 )                  \
    : ToolUtil (tool) {                                       \
        DELEGATE_NEW_IMPL (this,a1,a2,a3)                     \
    }                                                         \
  DELEGATE_BIG3_WITHOUT_CONSTRUCTOR_SELF ( t )                \

#define DELEGATE4_TOOL_UTIL(t,t1,t2,t3,t4)                    \
  t :: t ( Tool& tool, t1 a1, t2 a2, t3 a3, t4 a4 )           \
    : ToolUtil (tool) {                                       \
        DELEGATE_NEW_IMPL (this,a1,a2,a3,a4)                  \
    }                                                         \
  DELEGATE_BIG3_WITHOUT_CONSTRUCTOR_SELF ( t )                \

#define DELEGATE5_TOOL_UTIL(t,t1,t2,t3,t4,t5)                 \
  t :: t ( Tool& tool, t1 a1, t2 a2, t3 a3, t4 a4, t5 a5 )    \
    : ToolUtil (tool) {                                       \
        DELEGATE_NEW_IMPL (this,a1,a2,a3,a4,a5)               \
    }                                                         \
  DELEGATE_BIG3_WITHOUT_CONSTRUCTOR_SELF ( t )                \

#endif

#ifndef DILAY_FWD_GLM
#define DILAY_FWD_GLM

namespace glm { 
  namespace detail {
    template <typename T> struct tvec3;
    template <typename T> struct tvec4;
    template <typename T> struct tmat4x4;
  };
  typedef float                          mediump_float_t;
  typedef mediump_float_t                mediump_float;
  typedef detail::tvec3<mediump_float>   mediump_vec3;
  typedef detail::tvec4<mediump_float>   mediump_vec4;
  typedef mediump_vec3                   vec3;
  typedef mediump_vec4                   vec4;

  typedef detail::tmat4x4<mediump_float> mediump_mat4x4;
  typedef mediump_mat4x4                 mat4x4;
};

#endif

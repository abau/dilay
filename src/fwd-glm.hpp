#ifndef DILAY_FWD_GLM
#define DILAY_FWD_GLM

namespace glm { 
  namespace detail {
    template <typename T> struct tvec2;
    template <typename T> struct tvec3;
    template <typename T> struct tvec4;
    template <typename T> struct tmat4x4;
    template <typename T> struct tmat3x3;
  };
  typedef signed int                     mediump_int_t;
  typedef unsigned int                   mediump_uint_t;
  typedef float                          mediump_float_t;
  typedef mediump_int_t                  mediump_int;
  typedef mediump_uint_t                 mediump_uint;
  typedef mediump_float_t                mediump_float;
  typedef detail::tvec2<mediump_int>     mediump_ivec2;
  typedef detail::tvec2<mediump_uint>    mediump_uvec2;
  typedef detail::tvec3<mediump_float>   mediump_vec3;
  typedef detail::tvec4<mediump_float>   mediump_vec4;
  typedef mediump_ivec2                  ivec2;
  typedef mediump_uvec2                  uvec2;
  typedef mediump_vec3                   vec3;
  typedef mediump_vec4                   vec4;

  typedef detail::tmat4x4<mediump_float> mediump_mat4x4;
  typedef detail::tmat3x3<mediump_float> mediump_mat3x3;
  typedef mediump_mat4x4                 mat4x4;
  typedef mediump_mat3x3                 mat3x3;
};

#endif
